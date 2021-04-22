#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include "Hitable.h"
#include "Camera.h"
#include "ArSampler.h"

namespace Aurora
{

	class Tracer
	{
	private:

		class Setting
		{
		public:
			Camera::ptr m_camera;
			unsigned int m_maxDepth;
			unsigned int m_samplings;
			int m_width, m_height, m_channel;
			Float startFrame, endFrame, totalFrameTime;

			Setting() :m_maxDepth(50), m_samplings(10), m_channel(4), m_camera(nullptr) {}

		};

		Setting m_config;								// configuration.
		HitableList::ptr m_scene;						// Scene object lists.
		ASampler::ptr m_sampler;

	public:
		Tracer();
		~Tracer();

		int getWidth() const { return m_config.m_width; }
		int getHeight() const { return m_config.m_height; }
		int getChannel() const { return m_config.m_channel; }
		Float getTotalFrameTime() const { return m_config.totalFrameTime; }
		unsigned int getSamplings() const { return m_config.m_samplings; }
		unsigned int getRecursionDepth() const { return m_config.m_maxDepth; }
		Camera *getCamera() const { return m_config.m_camera.get(); }

		void addObjects(const Hitable::ptr &object);
		void initialize(int w, int h, int samplingNum, int depth);
		void beginFrame();
		void endFrame();

		void render(Float &totalTime);

	private:
		ASpectrum tracing(const ARay &r, Hitable *world, int depth);

		ASpectrum deNan(const ASpectrum &c);
	};

}

#endif // RENDERER_H
