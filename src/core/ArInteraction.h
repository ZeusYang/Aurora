#ifndef ARINTERACTION_H
#define ARINTERACTION_H

#include "ArAurora.h"
#include "ArMathUtils.h"

namespace Aurora
{
	class AInteraction
	{
	public:

		AInteraction() = default;
		AInteraction(const AVector3f &p) : p(p) {}
		AInteraction(const AVector3f &p, const AVector3f &wo) : p(p), wo(normalize(wo)) {}
		AInteraction(const AVector3f &p, const AVector3f &n, const AVector3f &wo)
			: p(p), wo(normalize(wo)), n(n) {}

		inline ARay spawnRay(const AVector3f &d) const
		{
			AVector3f o = p;
			return ARay(o, d, aInfinity);
		}

		inline ARay spawnRayTo(const AVector3f &p2) const
		{
			AVector3f origin = p;
			return ARay(origin, p2 - p, 1 - aShadowEpsilon);
		}

		inline ARay spawnRayTo(const AInteraction &it) const
		{
			AVector3f origin = p;
			AVector3f target = it.p;
			AVector3f d = target - origin;
			return ARay(origin, d, 1 - aShadowEpsilon);
		}

	public:
		AVector3f p;			//surface point
		AVector3f wo;			//outgoing direction
		AVector3f n;			//normal vector
	};

	class ASurfaceInteraction final : public AInteraction
	{
	public:

		ASurfaceInteraction() = default;
		ASurfaceInteraction(const AVector3f &p, const AVector2f &uv, const AVector3f &wo,
			const AVector3f &dpdu, const AVector3f &dpdv, const AShape *sh);

		ASpectrum Le(const AVector3f &w) const;

		void computeScatteringFunctions(const ARay &ray, MemoryArena &arena,
			bool allowMultipleLobes = false, ATransportMode mode = ATransportMode::aRadiance);

	public:
		AVector2f uv;
		AVector3f dpdu, dpdv;

		ABSDF* bsdf = nullptr;

		const AShape *shape = nullptr;
		const AHitable *hitable = nullptr;

	};
}

#endif