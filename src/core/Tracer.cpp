#include "Tracer.h"

#include "Hitable.h"
#include "Camera.h"
#include "Material.h"
#include "CosinePDF.h"
#include "HitablePDF.h"

#include <random>
#include <time.h>

using namespace std;

namespace Aurora
{

	Tracer::Tracer() {}

	Tracer::~Tracer() {}

	void Tracer::initialize(int w, int h, int samplingNum, int depth)
	{
		// Image width and height.
		m_config.m_width = w;
		m_config.m_height = h;

		//Sampler
		m_sampler = std::make_shared<ARandomSampler>(samplingNum);

		//Scene
		m_scene = std::make_shared<HitableList>();

		AVector2i res(w, h);
		AFilm::ptr film = std::make_shared<AFilm>(res, "../result.png");

		// Camera initialization.
		AVector3f lookfrom(0, 6, 21);
		AVector3f lookat(0, 0, 0);
		Float dist_to_focus = 10.0f;
		Float aperture = 0.0f;
		m_config.m_camera = std::make_shared<Camera>(film, lookfrom, lookat, 45,
			static_cast<Float>(m_config.m_width) / m_config.m_height,
			aperture, dist_to_focus);

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
		m_config.startFrame = clock();

		for (int y = 0; y < m_config.m_height; ++y)
		{
			for (int x = 0; x < m_config.m_width; ++x)
			{
				ASpectrum Li;
				AVector2i pRaster(x, y);
				m_sampler->startPixel(pRaster);
				//for (int sps = 0; sps < m_config.m_samplings; ++sps)
				do
				{
					auto sample = m_sampler->getCameraSample(pRaster);
					Float u = sample.pFilm.x / static_cast<Float>(m_config.m_width);
					Float v = sample.pFilm.y / static_cast<Float>(m_config.m_height);
					ARay ray = m_config.m_camera->getRay(u, v);
					Li += deNan(tracing(ray, m_scene.get(), 0));
				} while (m_sampler->startNextSample());

				// gamma correction & box filtering
				Li = sqrt(Li / m_sampler->getSamplingNumber());

				m_config.m_camera->m_film->setSpectrum(pRaster, Li);
			}
		}

		m_config.endFrame = clock();
		m_config.totalFrameTime = static_cast<Float>(m_config.endFrame - m_config.startFrame) / CLOCKS_PER_SEC;
		totalTime = m_config.totalFrameTime;

		m_config.m_camera->m_film->writeImageToFile();
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

	ASpectrum Tracer::tracing(const ARay &r, Hitable *world, int depth)
	{
		HitRecord rec;
		if (world->hit(r, rec))
		{
			ScatterRecord srec;
			Material* material = rec.m_material;
			ASpectrum emitted = material->emitted(r, rec, rec.m_texcoord.x, rec.m_texcoord.y, rec.m_position);
			if (depth < m_config.m_maxDepth && material->scatter(r, rec, srec))
			{
				if (srec.m_isSpecular)
				{
					return srec.m_attenuation * tracing(srec.m_scatterRay, world, depth + 1);
				}
				else
				{
					AVector3f dir;
					Float pdf_val;
					dir = srec.m_pdf->generate();
					pdf_val = srec.m_pdf->value(dir);
					ARay scattered = ARay(rec.m_position, normalize(dir));

					return emitted + srec.m_attenuation * material->scattering_pdf(r, rec, scattered)
						* tracing(scattered, world, depth + 1) / pdf_val;
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
}
