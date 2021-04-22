#include "ArInteraction.h"

#include "ArSpectrum.h"
//#include "ArHitable.h"
//#include "ArBxDF.h"

namespace Aurora
{
	ASurfaceInteraction::ASurfaceInteraction(const AVector3f &p, const AVector2f &uv, const AVector3f &wo,
		const AVector3f &dpdu, const AVector3f &dpdv, const AShape *sh)
		: AInteraction(p, normalize(cross(dpdu, dpdv)), wo), uv(uv), dpdu(dpdu), dpdv(dpdv), shape(sh) {}

	ASpectrum ASurfaceInteraction::Le(const AVector3f &w) const
	{
		//const AAreaLight *area = hitable->getAreaLight();
		//return area != nullptr ? area->L(*this, w) : ASpectrum(0.f);
		return ASpectrum(0.f);
	}

	void ASurfaceInteraction::computeScatteringFunctions(const ARay &ray, MemoryArena &arena,
		bool allowMultipleLobes, ATransportMode mode)
	{
		//hitable->computeScatteringFunctions(*this, arena, mode, allowMultipleLobes);
		return;
	}

}