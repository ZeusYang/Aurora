#ifndef AINTEGRATOR_H
#define AINTEGRATOR_H

#include "ArAurora.h"
#include "ArMathUtils.h"
#include "ArSampler.h"
#include "ArCamera.h"
#include "ArHitable.h"
#include "ArRtti.h"

namespace Aurora
{
	class AIntegrator : public AObject
	{
	public:
		typedef std::shared_ptr<AIntegrator> ptr;

		virtual ~AIntegrator() = default;

		virtual void preprocess(const AScene &scene) = 0;
		virtual void render(const AScene &scene) = 0;

		virtual AClassType getClassType() const override { return AClassType::AEIntegrator; }

	};

	class ASamplerIntegrator : public AIntegrator
	{
	public:
		typedef std::shared_ptr<ASamplerIntegrator> ptr;

		// SamplerIntegrator Public Methods
		ASamplerIntegrator(ACamera::ptr camera, ASampler::ptr sampler)
			: m_camera(camera), m_sampler(sampler) {}

		virtual void preprocess(const AScene &scene) override {}

		virtual void render(const AScene &scene) override;

		virtual ASpectrum Li(const ARay &ray, const AScene &scene,
			ASampler &sampler, MemoryArena &arena, int depth = 0) const = 0;

		ASpectrum specularReflect(const ARay &ray, const ASurfaceInteraction &isect,
			const AScene &scene, ASampler &sampler, MemoryArena &arena, int depth) const;

		ASpectrum specularTransmit(const ARay &ray, const ASurfaceInteraction &isect,
			const AScene &scene, ASampler &sampler, MemoryArena &arena, int depth) const;

	protected:
		ACamera::ptr m_camera;
		ASampler::ptr m_sampler;
	};

	class AWhittedIntegrator : public ASamplerIntegrator
	{
	public:
		typedef std::shared_ptr<AWhittedIntegrator> ptr;

		AWhittedIntegrator(const APropertyTreeNode &node);
		AWhittedIntegrator(int maxDepth, ACamera::ptr camera, ASampler::ptr sampler)
			: ASamplerIntegrator(camera, sampler), m_maxDepth(maxDepth) {}

		virtual ASpectrum Li(const ARay &ray, const AScene &scene,
			ASampler &sampler, MemoryArena &arena, int depth) const override;

		virtual std::string toString() const override { return "WhittedIntegrator[]"; }

		//virtual void activate() override
		//{
		//	m_sampler->activate();
		//	m_camera->activate();
		//}

	private:
		const int m_maxDepth;
	};

	ASpectrum uiformSampleAllLights(const AInteraction &it, const AScene &scene,
		MemoryArena &arena, ASampler &sampler, const std::vector<int> &nLightSamples);

	ASpectrum uniformSampleOneLight(const AInteraction &it, const AScene &scene,
		MemoryArena &arena, ASampler &sampler, const ADistribution1D *lightDistrib);

	ASpectrum estimateDirect(const AInteraction &it, const AVector2f &uShading, const ALight &light,
		const AVector2f &uLight, const AScene &scene, ASampler &sampler, MemoryArena &arena, bool specular = false);

}

#endif