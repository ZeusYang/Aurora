#ifndef AINTEGRATOR_H
#define AINTEGRATOR_H

#include "ArAurora.h"
#include "ArMathUtils.h"
#include "ArSampler.h"
#include "ArCamera.h"
#include "ArHitable.h"

namespace Aurora
{
	class AIntegrator
	{
	public:
		virtual ~AIntegrator() = default;
		virtual void render(const AScene &scene) = 0;
	};

	class ASamplerIntegrator : public AIntegrator
	{
	public:

		// SamplerIntegrator Public Methods
		ASamplerIntegrator(ACamera::ptr camera, ASampler::ptr sampler, const ABounds2i &pixelBounds)
			: m_camera(camera), m_sampler(sampler), m_pixelBounds(pixelBounds) {}

		virtual void preprocess(const AScene &scene, ASampler &sampler) {}

		virtual void render(const AScene &scene) override;

		virtual ASpectrum Li(const ARay &ray, const AScene &scene,
			ASampler &sampler, MemoryArena &arena, int depth = 0) const = 0;

		ASpectrum specularReflect(const ARay &ray, const ASurfaceInteraction &isect,
			const AScene &scene, ASampler &sampler, MemoryArena &arena, int depth) const;

		ASpectrum specularTransmit(const ARay &ray, const ASurfaceInteraction &isect,
			const AScene &scene, ASampler &sampler, MemoryArena &arena, int depth) const;

	protected:
		ACamera::ptr m_camera;

	private:
		ASampler::ptr m_sampler;
		const ABounds2i m_pixelBounds;
	};

	class AWhittedIntegrator : public ASamplerIntegrator
	{
	public:
		// WhittedIntegrator Public Methods
		AWhittedIntegrator(int maxDepth, ACamera::ptr camera,
			std::shared_ptr<ASampler> sampler,
			const ABounds2i &pixelBounds)
			: ASamplerIntegrator(camera, sampler, pixelBounds), m_maxDepth(maxDepth) {}

		virtual ASpectrum Li(const ARay &ray, const AScene &scene,
			ASampler &sampler, MemoryArena &arena, int depth) const override;

	private:
		const int m_maxDepth;
	};

	ASpectrum uiformSampleAllLights(const AInteraction &it, const AScene &scene,
		MemoryArena &arena, ASampler &sampler, const std::vector<int> &nLightSamples);

	ASpectrum uniformSampleOneLight(const AInteraction &it, const AScene &scene,
		MemoryArena &arena, ASampler &sampler);

	ASpectrum estimateDirect(const AInteraction &it, const AVector2f &uShading, const ALight &light,
		const AVector2f &uLight, const AScene &scene, ASampler &sampler, MemoryArena &arena, bool specular = false);

}

#endif