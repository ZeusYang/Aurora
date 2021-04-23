#ifndef ARPATH_INTEGRATOR_H
#define ARPATH_INTEGRATOR_H

#include "ArAurora.h"
#include "ArMathUtils.h"
#include "ArIntegrator.h"
#include "ArLightDistrib.h"

namespace Aurora
{
	class APathIntegrator : public ASamplerIntegrator 
	{
	public:

		APathIntegrator(int maxDepth, ACamera::ptr camera, ASampler::ptr sampler,
			const ABounds2i &pixelBounds, Float rrThreshold = 1,
			const std::string &lightSampleStrategy = "spatial");

		virtual void preprocess(const AScene &scene, ASampler &sampler) override;
		
		virtual ASpectrum Li(const ARay &ray, const AScene &scene, ASampler &sampler, 
			MemoryArena &arena, int depth) const override;

	private:
		// PathIntegrator Private Data
		const int m_maxDepth;
		const Float m_rrThreshold;
		const std::string m_lightSampleStrategy;
		std::unique_ptr<ALightDistribution> m_lightDistribution;
	};
}

#endif