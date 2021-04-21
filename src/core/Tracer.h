#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include "Hitable.h"

namespace Aurora
{

	class Camera;
	class Hitable;

	enum Background
	{
		PURE = 0, LERP = 1, SKYBOX = 2
	};

	class Tracer
	{
	private:

		class Setting
		{
		public:
			Camera *m_camera;
			int m_parallelForCpu;
			Background m_background;
			unsigned int m_maxDepth;
			unsigned int m_samplings;
			int m_width, m_height, m_channel;
			Float startFrame, endFrame, totalFrameTime;

			Setting() :m_maxDepth(50), m_samplings(10),
				m_channel(4), m_camera(nullptr), m_parallelForCpu(1), m_background(PURE) {}
			~Setting()
			{
				if (m_camera)delete m_camera;
				m_camera = nullptr;
			}
		};
		//BVHNode *m_root;								// BVH tree root node.
		Setting m_config;								// configuration.
		unsigned char *m_image;							// Render target.
		HitableList m_samplingList;                     // Important sampling list.
		HitableList m_scene;				// Scene object lists.

	public:
		Tracer();
		~Tracer();

		void setParallelThread(int way) { m_config.m_parallelForCpu = way; }
		void setSamplingNums(unsigned int samp) { m_config.m_samplings = samp; }
		void setRecursionDepth(unsigned int depth) { m_config.m_maxDepth = depth; }

		int getWidth() const { return m_config.m_width; }
		int getHeight() const { return m_config.m_height; }
		int getChannel() const { return m_config.m_channel; }
		Float getTotalFrameTime() const { return m_config.totalFrameTime; }
		unsigned int getSamplings() const { return m_config.m_samplings; }
		unsigned int getRecursionDepth() const { return m_config.m_maxDepth; }
		unsigned char *getImage() const { return m_image; }
		Camera *getCamera() const { return m_config.m_camera; }

		void addImportantSampling(Hitable *target);
		void addObjects(Hitable *target);
		void initialize(int w, int h, int c = 4);
		void setSkybox(const std::vector<unsigned int> &texUnits);
		void setSkybox(const std::string &path, const std::string &postfix);
		void beginFrame();
		void endFrame();

		unsigned char *render(Float &totalTime);

	private:
		AVector3f tracing(const ARay &r, Hitable *world, Hitable *light, int depth);
		void drawPixel(unsigned int x, unsigned int y, const AVector3f &color);

		void rawSerialRender(Hitable *scene);
		void parallelThreadRender(Hitable *scene);

		AVector3f deNan(const AVector3f &c);
	};

}

#endif // RENDERER_H
