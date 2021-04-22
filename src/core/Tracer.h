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

		class Setting
		{
		public:
			unsigned int m_maxDepth;
			int m_width, m_height, m_channel;
			Float startFrame, endFrame, totalFrameTime;

			Setting() :m_maxDepth(10), m_channel(4) {}

		};

		Setting m_config;								// configuration.
		HitableList::ptr m_scene;						// Scene object lists.
		ACamera::ptr m_camera;
		ASampler::ptr m_sampler;

	public:
		Tracer();
		~Tracer();

		int getWidth() const { return m_config.m_width; }
		int getHeight() const { return m_config.m_height; }
		int getChannel() const { return m_config.m_channel; }
		Float getTotalFrameTime() const { return m_config.totalFrameTime; }
		unsigned int getRecursionDepth() const { return m_config.m_maxDepth; }

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
