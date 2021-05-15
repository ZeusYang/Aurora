#ifndef AWHITTEDINTEGRATOR_H
#define AWHITTEDINTEGRATOR_H

#include "ArIntegrator.h"

namespace Aurora
{
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

	private:
		const int m_maxDepth;
	};
}

#endif