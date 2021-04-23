#include "ArMaterial.h"

#include "ArBSDF.h"
#include "ArMemory.h"
#include "ArSpectrum.h"
#include "ArInteraction.h"

namespace Aurora
{

	//-------------------------------------------AMirrorMaterial-------------------------------------

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