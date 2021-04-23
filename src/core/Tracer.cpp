#include "Tracer.h"

#include "Hitable.h"
#include "Material.h"
#include "PDF.h"

#include <random>
#include <time.h>

using namespace std;

namespace Aurora
{

	Tracer::Tracer() {}

	Tracer::~Tracer() {}

	void Tracer::initialize(int w, int h, int samplingNum, int depth,
		const AVector3f &eye, const AVector3f &target, Float fovy)
	{
		//Depth
		m_maxDepth = depth;
		m_width = w, m_height = h;

		//Sampler
		m_sampler = std::make_shared<ARandomSampler>(samplingNum);

		//Scene
		m_scene = std::make_shared<HitableList>();

		//Film
		AVector2i res(w, h);
		AFilm::ptr film = std::make_shared<AFilm>(res, "../result.png");

		//Camera
		{
			ABounds2f screen;
			Float frame = (Float)(w) / h;
			if (frame > 1.f)
			{
				screen.m_pMin.x = -frame;
				screen.m_pMax.x = frame;
				screen.m_pMin.y = -1.f;
				screen.m_pMax.y = 1.f;
			}
			else
			{
				screen.m_pMin.x = -1.f;
				screen.m_pMax.x = 1.f;
				screen.m_pMin.y = -1.f / frame;
				screen.m_pMax.y = 1.f / frame;
			}

			auto cameraToWorld = inverse(lookAt(eye, target, AVector3f(0, 1, 0)));
			m_camera = std::make_shared<APerspectiveCamera>(cameraToWorld, screen, fovy, film);
		}

		// clear something.
		endFrame();
	}

	void Tracer::addObjects(const Hitable::ptr &object)
	{
		m_scene->addObjects(object);
	}

	void Tracer::beginFrame()
	{
		m_scene->preRendering();
	}

	void Tracer::endFrame()
	{
		// clear scene objects.
	}

	void Tracer::render(Float &totalTime)
	{
		auto startFrame = clock();

		for (int y = 0; y < m_height; ++y)
		{
			for (int x = 0; x < m_width; ++x)
			{
				ASpectrum Li;
				AVector2i pRaster(x, y);

				m_sampler->startPixel(pRaster);
				do
				{
					auto sample = m_sampler->getCameraSample(pRaster);
					
					//Casting a ray to scene
					ARay ray;
					Float rayWeight = m_camera->castingRay(sample, ray);

					Li += deNan(tracing(ray, m_scene.get(), 0));
				} while (m_sampler->startNextSample());

				// gamma correction & box filtering
				Li = sqrt(Li / m_sampler->getSamplingNumber());

				m_camera->m_film->setSpectrum(pRaster, Li);
			}
		}

		auto endFrame = clock();
		totalTime = static_cast<Float>(endFrame - startFrame) / CLOCKS_PER_SEC;

		m_camera->m_film->writeImageToFile();
	}

	ASpectrum Tracer::deNan(const ASpectrum &c)
	{
		ASpectrum tmp = c;
		for (int i = 0; i < tmp.nSamples; ++i)
		{
			if (!(tmp[i] == tmp[i]))
				tmp[i] = 0;
		}
		return tmp;
	}

	ASpectrum Tracer::tracing(const ARay &r, Hitable *scene, int depth)
	{
		HitRecord rec;
		if (scene->hit(r, rec))
		{
			ScatterRecord srec;

			Material* material = rec.m_material;
			ASpectrum emitted = material->emitted(r, rec, rec.m_texcoord.x, rec.m_texcoord.y, rec.m_position);

			if (depth < m_maxDepth && material->scatter(r, rec, srec))
			{
				if (srec.m_isSpecular)
				{
					return srec.m_attenuation * tracing(srec.m_scatterRay, scene, depth + 1);
				}
				else
				{
					AVector3f dir;
					Float pdf_val;
					dir = srec.m_pdf->generate();
					pdf_val = srec.m_pdf->value(dir);
					ARay scattered = ARay(rec.m_position, normalize(dir));

					return emitted + srec.m_attenuation * material->scattering_pdf(r, rec, scattered)
						* tracing(scattered, scene, depth + 1) / pdf_val;
				}
			}
			else
				return emitted;
		}
		else
		{
			// background color.
			Float t = 0.5f * (r.direction().y + 1.0f);
			auto tmp = AVector3f(1.0f, 1.0f, 1.0f) * (1.0f - t) + AVector3f(0.5f, 0.7f, 1.0f) * t;
			Float rgb[3] = { tmp.x, tmp.y, tmp.z };
			ASpectrum bg = ARGBSpectrum::fromRGB(rgb);
			return bg;
		}
	}

	//ASpectrum Tracer::tracing(const ARay &r, Hitable *scene, int depth)
	//{
	//	HitRecord rec;
	//	if (scene->hit(r, rec))
	//	{
	//		ScatterRecord srec;

	//		Material* material = rec.m_material;
	//		ASpectrum emitted = material->emitted(r, rec, rec.m_texcoord.x, rec.m_texcoord.y, rec.m_position);

	//		if (depth < m_maxDepth && material->scatter(r, rec, srec))
	//		{
	//			if (srec.m_isSpecular)
	//			{
	//				return srec.m_attenuation * tracing(srec.m_scatterRay, scene, depth + 1);
	//			}
	//			else
	//			{
	//				AVector3f dir;
	//				Float pdf_val;
	//				dir = srec.m_pdf->generate();
	//				pdf_val = srec.m_pdf->value(dir);
	//				ARay scattered = ARay(rec.m_position, normalize(dir));

	//				return emitted + srec.m_attenuation * material->scattering_pdf(r, rec, scattered)
	//					* tracing(scattered, scene, depth + 1) / pdf_val;
	//			}
	//		}
	//		else
	//			return emitted;
	//	}
	//	else
	//	{
	//		// background color.
	//		Float t = 0.5f * (r.direction().y + 1.0f);
	//		auto tmp = AVector3f(1.0f, 1.0f, 1.0f) * (1.0f - t) + AVector3f(0.5f, 0.7f, 1.0f) * t;
	//		Float rgb[3] = { tmp.x, tmp.y, tmp.z };
	//		ASpectrum bg = ARGBSpectrum::fromRGB(rgb);
	//		return bg;
	//	}
	//}
}
