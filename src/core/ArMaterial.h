#ifndef ARMATERIAL_H
#define ARMATERIAL_H

#include "ArAurora.h"
#include "ArSpectrum.h"
#include "ArMathUtils.h"
#include "ArRtti.h"

namespace Aurora
{

	class AMaterial : public AObject
	{
	public:
		typedef std::shared_ptr<AMaterial> ptr;

		AMaterial() = default;
		virtual ~AMaterial() = default;

		virtual void computeScatteringFunctions(ASurfaceInteraction &si, MemoryArena &arena,
			ATransportMode mode, bool allowMultipleLobes) const = 0;

		virtual AClassType getClassType() const override { return AClassType::AEMaterial; }

	};

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

	//class AGlassMaterial final : public AMaterial
	//{
	//public:
	//	typedef std::shared_ptr<AGlassMaterial> ptr;

	//	AGlassMaterial(const ASpectrum &Kr, const ASpectrum &Kt, Float uRoughness, Float vRoughness, Float index) 
	//		: m_Kr(Kr), m_Kt(Kt), m_uRoughness(uRoughness), m_vRoughness(vRoughness), m_index(index) {}

	//	virtual void computeScatteringFunctions(ASurfaceInteraction &si, MemoryArena &arena,
	//		ATransportMode mode, bool allowMultipleLobes) const override;

	//private:
	//	ASpectrum m_Kr, m_Kt;
	//	Float m_uRoughness, m_vRoughness, m_index;
	//};

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