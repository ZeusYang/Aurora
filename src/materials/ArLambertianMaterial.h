#ifndef ARLAMBERTIANMATERIAL_H
#define ARLAMBERTIANMATERIAL_H

#include "ArMaterial.h"

namespace Aurora
{
	class ALambertianMaterial final : public AMaterial
	{
	public:
		typedef std::shared_ptr<ALambertianMaterial> ptr;

		ALambertianMaterial(const APropertyTreeNode &node);
		ALambertianMaterial(const ASpectrum &r) : m_Kr(r) {}

		virtual void computeScatteringFunctions(ASurfaceInteraction &si, MemoryArena &arena,
			ATransportMode mode, bool allowMultipleLobes) const override;

		virtual std::string toString() const override { return "MirrorMaterial[]"; }

	private:
		ASpectrum m_Kr;
	};
}

#endif