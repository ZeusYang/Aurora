#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include "Hitable.h"
#include "ArCamera.h"
#include "ArSampler.h"

namespace Aurora
{

	class Tracer
	{
	private:

		HitableList::ptr m_scene;
		ACamera::ptr m_camera;
		ASampler::ptr m_sampler;

		int m_width, m_height;
		unsigned int m_maxDepth = 10;

	public:
		Tracer();
		~Tracer();

		void initialize(int w, int h, int samplingNum, int depth,
			const AVector3f &eye, const AVector3f &target, Float fovy);

		void addObjects(const Hitable::ptr &object);

		void beginFrame();
		void endFrame();

		void render(Float &totalTime);

	private:
		ASpectrum tracing(const ARay &r, Hitable *world, int depth);

		ASpectrum deNan(const ASpectrum &c);
	};

}

#endif // RENDERER_H
