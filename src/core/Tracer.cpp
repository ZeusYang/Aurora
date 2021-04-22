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

	Tracer::Tracer() :m_image(nullptr) { }

	Tracer::~Tracer()
	{
		if (m_image) 
			delete m_image;
		m_image = nullptr;
	}

	void Tracer::initialize(int w, int h, int c)
	{
		// Image width and height.
		m_config.m_width = w;
		m_config.m_height = h;
		if (m_image != nullptr)
			delete m_image;
		if (m_config.m_camera != nullptr)
			delete m_config.m_camera;

		m_scene = std::make_shared<HitableList>();

		// Camera initialization.
		AVector3f lookfrom(0, 6, 21);
		AVector3f lookat(0, 0, 0);
		Float dist_to_focus = 10.0f;
		Float aperture = 0.0f;
		m_config.m_camera = new Camera(lookfrom, lookat, 45,
			static_cast<Float>(m_config.m_width) / m_config.m_height,
			aperture, dist_to_focus);

		// Image buffer.
		m_image = new unsigned char[m_config.m_width * m_config.m_height * m_config.m_channel];

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

	unsigned char *Tracer::render(Float &totalTime)
	{
		m_config.startFrame = clock();

		for (int row = m_config.m_height - 1; row >= 0; --row)
		{
			for (int col = 0; col < m_config.m_width; ++col)
			{
				AVector3f color;
				for (int sps = 0; sps < m_config.m_samplings; ++sps)
				{
					Float u = static_cast<Float>(col + drand48()) / static_cast<Float>(m_config.m_width);
					Float v = static_cast<Float>(row + drand48()) / static_cast<Float>(m_config.m_height);
					ARay ray = m_config.m_camera->getRay(u, v);
					color += deNan(tracing(ray, m_scene.get(), 0));
				}
				color /= static_cast<Float>(m_config.m_samplings);
				// gamma correction.
				color = AVector3f(sqrt(color.x), sqrt(color.y), sqrt(color.z));
				if (color.x > 1.0f) color.x = 1.0f;
				if (color.y > 1.0f) color.y = 1.0f;
				if (color.z > 1.0f) color.z = 1.0f;
				drawPixel(col, row, color);
			}
		}
		m_config.endFrame = clock();
		m_config.totalFrameTime = static_cast<Float>(m_config.endFrame - m_config.startFrame) / CLOCKS_PER_SEC;
		totalTime = m_config.totalFrameTime;

		return m_image;
	}

	void Tracer::drawPixel(unsigned int x, unsigned int y, const AVector3f &color)
	{
		if (x < 0 || x >= m_config.m_width || y < 0 || y >= m_config.m_height)
			return;
		unsigned int index = (y * m_config.m_width + x) * m_config.m_channel;
		m_image[index + 0] = static_cast<unsigned char>(255.0f * color.x);
		m_image[index + 1] = static_cast<unsigned char>(255.0f * color.y);
		m_image[index + 2] = static_cast<unsigned char>(255.0f * color.z);
		m_image[index + 3] = static_cast<unsigned char>(255.0f);
	}

	AVector3f Tracer::deNan(const AVector3f &c)
	{
		AVector3f temp = c;
		if (!(temp.x == temp.x))temp.x = 0;
		if (!(temp.y == temp.y))temp.y = 0;
		if (!(temp.z == temp.z))temp.z = 0;
		return temp;
	}

	AVector3f Tracer::tracing(const ARay &r, Hitable *world, int depth)
	{
		HitRecord rec;
		if (world->hit(r, rec))
		{
			ScatterRecord srec;
			Material* material = rec.m_material;
			AVector3f emitted = material->emitted(r, rec, rec.m_texcoord.x, rec.m_texcoord.y, rec.m_position);
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
			AVector3f ret;
			Float t = 0.5f * (r.direction().y + 1.0f);
			ret = AVector3f(1.0f, 1.0f, 1.0f) * (1.0f - t) + AVector3f(0.5f, 0.7f, 1.0f) * t;
			return ret;
		}
	}
}
