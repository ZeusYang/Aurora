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

		APathIntegrator(const APropertyTreeNode &props);

		APathIntegrator(int maxDepth, ACamera::ptr camera, ASampler::ptr sampler,
			Float rrThreshold = 1, const std::string &lightSampleStrategy = "spatial");

		virtual void preprocess(const AScene &scene) override;
		
		virtual ASpectrum Li(const ARay &ray, const AScene &scene, ASampler &sampler, 
			MemoryArena &arena, int depth) const override;

		virtual std::string toString() const override { return "PathIntegrator[]"; }

	private:
		// PathIntegrator Private Data
		int m_maxDepth;
		Float m_rrThreshold;
		std::string m_lightSampleStrategy;
		std::unique_ptr<ALightDistribution> m_lightDistribution;
	};

}

#endif