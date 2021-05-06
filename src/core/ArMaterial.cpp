#include "ArMaterial.h"

#include "ArBSDF.h"
#include "ArMemory.h"
#include "ArSpectrum.h"
#include "ArInteraction.h"

namespace Aurora
{

	//-------------------------------------------AMirrorMaterial-------------------------------------

	AURORA_REGISTER_CLASS(AMirrorMaterial, "Mirror")

	AMirrorMaterial::AMirrorMaterial(const APropertyTreeNode &node)
	{
		const auto &props = node.getPropertyList();
		AVector3f _kr = props.getVector3f("R");
		Float _tmp[] = { _kr.x, _kr.y, _kr.z };
		m_Kr = ASpectrum::fromRGB(_tmp);
	}

	void AMirrorMaterial::computeScatteringFunctions(ASurfaceInteraction &si, MemoryArena &arena,
		ATransportMode mode, bool allowMultipleLobes) const
	{
		si.bsdf = ARENA_ALLOC(arena, ABSDF)(si);
		ASpectrum R = m_Kr;
		if (!R.isBlack())
		{
			si.bsdf->add(ARENA_ALLOC(arena, ASpecularReflection)(
				R, ARENA_ALLOC(arena, AFresnelNoOp)()));
		}
	}

	//-------------------------------------------ALambertianMaterial-------------------------------------

	AURORA_REGISTER_CLASS(ALambertianMaterial, "Lambertian")

	ALambertianMaterial::ALambertianMaterial(const APropertyTreeNode &node)
	{
		const auto &props = node.getPropertyList();
		AVector3f _kr = props.getVector3f("R");
		Float _tmp[] = { _kr.x, _kr.y, _kr.z };
		m_Kr = ASpectrum::fromRGB(_tmp);
	}

	void ALambertianMaterial::computeScatteringFunctions(ASurfaceInteraction &si, MemoryArena &arena,
		ATransportMode mode, bool allowMultipleLobes) const
	{
		si.bsdf = ARENA_ALLOC(arena, ABSDF)(si);
		ASpectrum R = m_Kr;
		if (!R.isBlack())
		{
			si.bsdf->add(ARENA_ALLOC(arena, ALambertianReflection)(R));
		}
	}
}