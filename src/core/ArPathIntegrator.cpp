#include "ArPathIntegrator.h"

#include "ArScene.h"
#include "ArBSDF.h"

namespace Aurora
{
	AURORA_REGISTER_CLASS(APathIntegrator, "Path")

	APathIntegrator::APathIntegrator(const APropertyTreeNode &node)
		: ASamplerIntegrator(nullptr, nullptr), m_maxDepth(node.getPropertyList().getInteger("Depth", 2))
	{

	}

	APathIntegrator::APathIntegrator(int maxDepth, ACamera::ptr camera, ASampler::ptr sampler,
		Float rrThreshold, const std::string &lightSampleStrategy)
		: ASamplerIntegrator(camera, sampler), m_maxDepth(maxDepth),
		m_rrThreshold(rrThreshold), m_lightSampleStrategy(lightSampleStrategy) {}

	void APathIntegrator::preprocess(const AScene &scene) 
	{
		m_lightDistribution = createLightSampleDistribution(m_lightSampleStrategy, scene);
	}

	ASpectrum APathIntegrator::Li(const ARay &r, const AScene &scene, ASampler &sampler,
		MemoryArena &arena, int depth) const 
	{
		ASpectrum L(0.f), beta(1.f);
		ARay ray(r);

		bool specularBounce = false;
		int bounces;
		// Added after book publication: etaScale tracks the accumulated effect
		// of radiance scaling due to rays passing through refractive
		// boundaries (see the derivation on p. 527 of the third edition). We
		// track this value in order to remove it from beta when we apply
		// Russian roulette; this is worthwhile, since it lets us sometimes
		// avoid terminating refracted rays that are about to be refracted back
		// out of a medium and thus have their beta value increased.
		Float etaScale = 1;

		for (bounces = 0;; ++bounces) 
		{
			// Find next path vertex and accumulate contribution

			// Intersect _ray_ with scene and store intersection in _isect_
			ASurfaceInteraction isect;
			bool foundIntersection = scene.hit(ray, isect);

			// Possibly add emitted light at intersection
			if (bounces == 0 || specularBounce) 
			{
				// Add emitted light at path vertex or from the environment
				if (foundIntersection) 
				{
					L += beta * isect.Le(-ray.direction());
				}
				else 
				{
					for (const auto &light : scene.m_infiniteLights)
						L += beta * light->Le(ray);
				}
			}

			// Terminate path if ray escaped or _maxDepth_ was reached
			if (!foundIntersection || bounces >= m_maxDepth) 
				break;

			// Compute scattering functions and skip over medium boundaries
			isect.computeScatteringFunctions(ray, arena, true);

			if (!isect.bsdf) 
			{
				ray = isect.spawnRay(ray.direction());
				bounces--;
				continue;
			}

			const ADistribution1D *distrib = m_lightDistribution->lookup(isect.p);

			// Sample illumination from lights to find path contribution.
			// (But skip this for perfectly specular BSDFs.)
			if (isect.bsdf->numComponents(ABxDFType(BSDF_ALL & ~BSDF_SPECULAR)) > 0) 
			{
				//++totalPaths;
				ASpectrum Ld = beta * uniformSampleOneLight(isect, scene, arena, sampler, distrib);
				//if (Ld.isBlack()) 
				//	++zeroRadiancePaths;
				//CHECK_GE(Ld.y(), 0.f);
				L += Ld;
			}

			// Sample BSDF to get new path direction
			AVector3f wo = -ray.direction(), wi;
			Float pdf;
			ABxDFType flags;
			ASpectrum f = isect.bsdf->sample_f(wo, wi, sampler.get2D(), pdf, flags, BSDF_ALL);
			
			if (f.isBlack() || pdf == 0.f) 
				break;
			beta *= f * absDot(wi, isect.n) / pdf;

			specularBounce = (flags & BSDF_SPECULAR) != 0;
			if ((flags & BSDF_SPECULAR) && (flags & BSDF_TRANSMISSION)) 
			{
				Float eta = isect.bsdf->m_eta;
				// Update the term that tracks radiance scaling for refraction
				// depending on whether the ray is entering or leaving the
				// medium.
				etaScale *= (dot(wo, isect.n) > 0) ? (eta * eta) : 1 / (eta * eta);
			}
			ray = isect.spawnRay(wi);

			// Possibly terminate the path with Russian roulette.
			// Factor out radiance scaling due to refraction in rrBeta.
			ASpectrum rrBeta = beta * etaScale;
			if (rrBeta.maxComponentValue() < m_rrThreshold && bounces > 3) 
			{
				Float q = glm::max((Float).05f, 1 - rrBeta.maxComponentValue());
				if (sampler.get1D() < q) 
					break;
				beta /= 1 - q;
			}
		}

		//ReportValue(pathLength, bounces);
		return L;
	}

}