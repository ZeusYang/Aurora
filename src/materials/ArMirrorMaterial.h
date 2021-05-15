#ifndef ARMIRRORMATERIAL_H
#define ARMIRRORMATERIAL_H

#include "ArMaterial.h"

namespace Aurora
{
	class AMirrorMaterial final : public AMaterial
	{
	public:
		typedef std::shared_ptr<AMirrorMaterial> ptr;

		AMirrorMaterial(const APropertyTreeNode &node);
		AMirrorMaterial(const ASpectrum &r) : m_Kr(r) {}

		virtual void computeScatteringFunctions(ASurfaceInteraction &si, MemoryArena &arena,
			ATransportMode mode, bool allowMultipleLobes) const override;

		virtual std::string toString() const override { return "MirrorMaterial[]"; }

	private:
		ASpectrum m_Kr;
	};
}

#endif