#include "ArIntegrator.h"

#include "ArBSDF.h"
#include "ArScene.h"
#include "ArMemory.h"
#include "ArReporter.h"
#include "ArLightDistrib.h"
#include "ArParallel.h"

namespace Aurora
{
	//-------------------------------------------ASamplerIntegrator-------------------------------------

	void ASamplerIntegrator::render(const AScene &scene)
	{
		AVector2i resolution = m_camera->m_film->getResolution();

		auto &sampler = m_sampler;

		// Compute number of tiles, _nTiles_, to use for parallel rendering
		ABounds2i sampleBounds = m_camera->m_film->getSampleBounds();
		AVector2i sampleExtent = sampleBounds.diagonal();
		constexpr int tileSize = 16;
		AVector2i nTiles((sampleExtent.x + tileSize - 1) / tileSize, (sampleExtent.y + tileSize - 1) / tileSize);

		AReporter reporter(nTiles.x * nTiles.y, "Rendering");
	 	AParallelUtils::parallelFor((size_t)0, (size_t)(nTiles.x * nTiles.y), [&](const size_t &t)
		{
			
			AVector2i tile(t % nTiles.x, t / nTiles.x);
			MemoryArena arena;

			// Get sampler instance for tile
			int seed = t;
			std::unique_ptr<ASampler> tileSampler = sampler->clone(seed);

			// Compute sample bounds for tile
			int x0 = sampleBounds.m_pMin.x + tile.x * tileSize;
			int x1 = glm::min(x0 + tileSize, sampleBounds.m_pMax.x);
			int y0 = sampleBounds.m_pMin.y + tile.y * tileSize;
			int y1 = glm::min(y0 + tileSize, sampleBounds.m_pMax.y);
			ABounds2i tileBounds(AVector2i(x0, y0), AVector2i(x1, y1));
			LOG(INFO) << "Starting image tile " << tileBounds;

			// Get _FilmTile_ for tile
			std::unique_ptr<AFilmTile> filmTile = m_camera->m_film->getFilmTile(tileBounds);

			// Loop over pixels in tile to render them
			for (AVector2i pixel : tileBounds)
			{
				tileSampler->startPixel(pixel);

				do
				{
					// Initialize _CameraSample_ for current sample
					ACameraSample cameraSample = tileSampler->getCameraSample(pixel);

					// Generate camera ray for current sample
					ARay ray;
					Float rayWeight = m_camera->castingRay(cameraSample, ray);

					// Evaluate radiance along camera ray
					ASpectrum L(0.f);
					if (rayWeight > 0)
					{
						L = Li(ray, scene, *tileSampler, arena);
					}

					// Issue warning if unexpected radiance value returned
					if (L.hasNaNs())
					{
						LOG(ERROR) << stringPrintf(
							"Not-a-number radiance value returned "
							"for pixel (%d, %d), sample %d. Setting to black.",
							pixel.x, pixel.y,
							(int)tileSampler->currentSampleNumber());
						L = ASpectrum(0.f);
					}
					else if (L.y() < -1e-5)
					{
						LOG(ERROR) << stringPrintf(
							"Negative luminance value, %f, returned "
							"for pixel (%d, %d), sample %d. Setting to black.",
							L.y(), pixel.x, pixel.y,
							(int)tileSampler->currentSampleNumber());
						L = ASpectrum(0.f);
					}
					else if (std::isinf(L.y()))
					{
						LOG(ERROR) << stringPrintf(
							"Infinite luminance value returned "
							"for pixel (%d, %d), sample %d. Setting to black.",
							pixel.x, pixel.y,
							(int)tileSampler->currentSampleNumber());
						L = ASpectrum(0.f);
					}
					VLOG(1) << "Camera sample: " << cameraSample << " -> ray: " << ray << " -> L = " << L;

					// Add camera ray's contribution to image
					filmTile->addSample(cameraSample.pFilm, L, rayWeight);

					// Free _MemoryArena_ memory from computing image sample value
					arena.Reset();

				} while (tileSampler->startNextSample());
			}
			LOG(INFO) << "Finished image tile " << tileBounds;

			m_camera->m_film->mergeFilmTile(std::move(filmTile));
			reporter.update();
			
		}, AExecutionPolicy::APARALLEL);

		reporter.done();

		LOG(INFO) << "Rendering finished";

		m_camera->m_film->writeImageToFile();

	}

	ASpectrum ASamplerIntegrator::specularReflect(const ARay &ray, const ASurfaceInteraction &isect,
		const AScene &scene, ASampler &sampler, MemoryArena &arena, int depth) const
	{
		// Compute specular reflection direction _wi_ and BSDF value
		AVector3f wo = isect.wo, wi;
		Float pdf;
		ABxDFType type = ABxDFType(BSDF_REFLECTION | BSDF_SPECULAR);

		ASpectrum f = isect.bsdf->sample_f(wo, wi, sampler.get2D(), pdf, type);

		// Return contribution of specular reflection
		const AVector3f &ns = isect.n;

		if (pdf > 0.f && !f.isBlack() && absDot(wi, ns) != 0.f)
		{
			// Compute ray differential _rd_ for specular reflection
			ARay rd = isect.spawnRay(wi);
			return f * Li(rd, scene, sampler, arena, depth + 1) * absDot(wi, ns) / pdf;
		}
		else
		{
			return ASpectrum(0.f);
		}
	}

	ASpectrum ASamplerIntegrator::specularTransmit(const ARay &ray, const ASurfaceInteraction &isect,
		const AScene &scene, ASampler &sampler, MemoryArena &arena, int depth) const
	{
		AVector3f wo = isect.wo, wi;
		Float pdf;
		const AVector3f &p = isect.p;
		const ABSDF &bsdf = *(isect.bsdf);
		ABxDFType sampledType = ABxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR);
		ASpectrum f = bsdf.sample_f(wo, wi, sampler.get2D(), pdf, sampledType);
		ASpectrum L = ASpectrum(0.f);
		AVector3f ns = isect.n;

		if (pdf > 0.f && !f.isBlack() && absDot(wi, ns) != 0.f)
		{
			// Compute ray differential _rd_ for specular transmission
			ARay rd = isect.spawnRay(wi);
			L = f * Li(rd, scene, sampler, arena, depth + 1) * absDot(wi, ns) / pdf;
		}
		return L;
	}

	//-------------------------------------------AWhittedIntegrator-------------------------------------

	AWhittedIntegrator::AWhittedIntegrator(const APropertyTreeNode &node)
		:ASamplerIntegrator(nullptr, nullptr), m_maxDepth(node.getPropertyList().getInteger("Depth", 2)) 
	{
		//Sampler
		{
			const auto &samplerNode = node.getPropertyChild("Sampler");
			m_sampler = ASampler::ptr(static_cast<ASampler*>(AObjectFactory::createInstance(
				samplerNode.getTypeName(), samplerNode)));
		}
		//Camera
		{
			const auto &cameraNode = node.getPropertyChild("Camera");
			m_camera = ACamera::ptr(static_cast<ACamera*>(AObjectFactory::createInstance(
				cameraNode.getTypeName(), cameraNode)));
		}

		activate();
	}

	ASpectrum AWhittedIntegrator::Li(const ARay &ray, const AScene &scene,
		ASampler &sampler, MemoryArena &arena, int depth) const
	{
		ASpectrum L(0.);

		ASurfaceInteraction isect;

		// No intersection found, just return lights emission
		if (!scene.hit(ray, isect))
		{
			for (const auto &light : scene.m_lights)
				L += light->Le(ray);
			return L;
		}

		// Compute emitted and reflected light at ray intersection point

		// Initialize common variables for Whitted integrator
		const AVector3f &n = isect.n;
		AVector3f wo = isect.wo;

		// Calculate BSDF function for surface interaction
		isect.computeScatteringFunctions(ray, arena);

		// There is no bsdf funcion
		if (!isect.bsdf)
		{
			return Li(isect.spawnRay(ray.direction()), scene, sampler, arena, depth);
		}

		// Compute emitted light if ray hit an area light source -> Le (emission term)
		L += isect.Le(wo);

		// Add contribution of each light source -> shadow ray
		for (const auto &light : scene.m_lights)
		{
			AVector3f wi;
			Float pdf;
			AVisibilityTester visibility;
			ASpectrum Li = light->sample_Li(isect, sampler.get2D(), wi, pdf, visibility);

			if (Li.isBlack() || pdf == 0)
				continue;

			ASpectrum f = isect.bsdf->f(wo, wi);
			if (!f.isBlack() && visibility.unoccluded(scene))
			{
				L += f * Li * absDot(wi, n) / pdf;
			}
		}

		if (depth + 1 < m_maxDepth)
		{
			// Trace rays for specular reflection and refraction
			L += specularReflect(ray, isect, scene, sampler, arena, depth);
			L += specularTransmit(ray, isect, scene, sampler, arena, depth);
		}

		return L;
	}

	//------------------------------------------Utility functions-------------------------------------

	ASpectrum uiformSampleAllLights(const AInteraction &it, const AScene &scene,
		MemoryArena &arena, ASampler &sampler, const std::vector<int> &nLightSamples)
	{
		ASpectrum L(0.f);
		for (size_t j = 0; j < scene.m_lights.size(); ++j)
		{
			// Accumulate contribution of _j_th light to _L_
			const ALight::ptr &light = scene.m_lights[j];
			int nSamples = nLightSamples[j];
			const AVector2f *uLightArray = sampler.get2DArray(nSamples);
			const AVector2f *uScatteringArray = sampler.get2DArray(nSamples);

			if (!uLightArray || !uScatteringArray)
			{
				// Use a single sample for illumination from _light_
				AVector2f uLight = sampler.get2D();
				AVector2f uScattering = sampler.get2D();
				L += estimateDirect(it, uScattering, *light, uLight, scene, sampler, arena);
			}
			else
			{
				// Estimate direct lighting using sample arrays
				ASpectrum Ld(0.f);
				for (int k = 0; k < nSamples; ++k)
				{
					Ld += estimateDirect(it, uScatteringArray[k], *light, uLightArray[k], scene, sampler, arena);
				}
				L += Ld / nSamples;
			}
		}
		return L;
	}

	ASpectrum uniformSampleOneLight(const AInteraction &it, const AScene &scene,
		MemoryArena &arena, ASampler &sampler, const ADistribution1D *lightDistrib)
	{
		// Randomly choose a single light to sample, _light_
		int nLights = int(scene.m_lights.size());

		if (nLights == 0)
			return ASpectrum(0.f);

		int lightNum;
		Float lightPdf;

		if (lightDistrib != nullptr) 
		{
			lightNum = lightDistrib->sampleDiscrete(sampler.get1D(), &lightPdf);
			if (lightPdf == 0) 
				return ASpectrum(0.f);
		}
		else 
		{
			lightNum = glm::min((int)(sampler.get1D() * nLights), nLights - 1);
			lightPdf = Float(1) / nLights;
		}

		const ALight::ptr &light = scene.m_lights[lightNum];
		AVector2f uLight = sampler.get2D();
		AVector2f uScattering = sampler.get2D();

		return estimateDirect(it, uScattering, *light, uLight, scene, sampler, arena) / lightPdf;
	}

	ASpectrum estimateDirect(const AInteraction &it, const AVector2f &uScattering, const ALight &light,
		const AVector2f &uLight, const AScene &scene, ASampler &sampler, MemoryArena &arena, bool specular)
	{
		ABxDFType bsdfFlags = specular ? BSDF_ALL : ABxDFType(BSDF_ALL & ~BSDF_SPECULAR);

		ASpectrum Ld(0.f);
		// Sample light source with multiple importance sampling
		AVector3f wi;
		Float lightPdf = 0, scatteringPdf = 0;
		AVisibilityTester visibility;
		ASpectrum Li = light.sample_Li(it, uLight, wi, lightPdf, visibility);

		if (lightPdf > 0 && !Li.isBlack())
		{
			// Compute BSDF or phase function's value for light sample
			ASpectrum f;
			// Evaluate BSDF for light sampling strategy
			const ASurfaceInteraction &isect = (const ASurfaceInteraction &)it;
			f = isect.bsdf->f(isect.wo, wi, bsdfFlags) * absDot(wi, isect.n);

			scatteringPdf = isect.bsdf->pdf(isect.wo, wi, bsdfFlags);

			if (!f.isBlack())
			{
				// Compute effect of visibility for light source sample
				if (!visibility.unoccluded(scene))
				{
					Li = ASpectrum(0.f);
				}

				// Add light's contribution to reflected radiance
				if (!Li.isBlack())
				{
					if (isDeltaLight(light.m_flags))
					{
						Ld += f * Li / lightPdf;
					}
					else
					{
						Float weight = powerHeuristic(1, lightPdf, 1, scatteringPdf);
						Ld += f * Li * weight / lightPdf;
					}
				}
			}
		}

		// Sample BSDF with multiple importance sampling
		if (!isDeltaLight(light.m_flags))
		{
			ASpectrum f;
			bool sampledSpecular = false;
			// Sample scattered direction for surface interactions
			ABxDFType sampledType;
			const ASurfaceInteraction &isect = (const ASurfaceInteraction &)it;
			f = isect.bsdf->sample_f(isect.wo, wi, uScattering, scatteringPdf, bsdfFlags, sampledType);
			f *= absDot(wi, isect.n);
			sampledSpecular = (sampledType & BSDF_SPECULAR) != 0;

			if (!f.isBlack() && scatteringPdf > 0)
			{
				// Account for light contributions along sampled direction _wi_
				Float weight = 1;
				if (!sampledSpecular)
				{
					lightPdf = light.pdf_Li(it, wi);
					if (lightPdf == 0) return Ld;
					weight = powerHeuristic(1, scatteringPdf, 1, lightPdf);
				}

				// Find intersection and compute transmittance
				ASurfaceInteraction lightIsect;
				ARay ray = it.spawnRay(wi);
				ASpectrum Tr(1.f);
				bool foundSurfaceInteraction = scene.hit(ray, lightIsect);

				// Add light contribution from material sampling
				ASpectrum Li(0.f);
				if (foundSurfaceInteraction)
				{
					if (lightIsect.hitable->getAreaLight() == &light)
						Li = lightIsect.Le(-wi);
				}
				else
				{
					Li = light.Le(ray);
				}
				if (!Li.isBlack())
					Ld += f * Li * Tr * weight / scatteringPdf;
			}
		}
		return Ld;
	}

	AURORA_REGISTER_CLASS(AWhittedIntegrator, "Whitted")

}