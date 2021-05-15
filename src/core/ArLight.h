#ifndef ARLIGHT_H
#define ARLIGHT_H

#include "ArAurora.h"
#include "ArMathUtils.h"
#include "ArShape.h"
#include "ArSpectrum.h"
#include "ArTransform.h"
#include "ArInteraction.h"
#include "ArRtti.h"

namespace Aurora
{
	enum class ALightFlags : int
	{
		ALightDeltaPosition = 1,
		ALightDeltaDirection = 2,
		ALightArea = 4,
		ALightInfinite = 8
	};

	inline bool isDeltaLight(int flags)
	{
		return flags & (int)ALightFlags::ALightDeltaPosition 
			|| flags & (int)ALightFlags::ALightDeltaDirection;
	}

	class ALight : public AObject
	{
	public:
		typedef std::shared_ptr<ALight> ptr;

		ALight(const APropertyList &props);
		ALight(int flags, const ATransform &LightToWorld, int nSamples = 1);

		virtual ~ALight();

		virtual ASpectrum power() const = 0;

		virtual void preprocess(const AScene &scene) {}

		virtual ASpectrum sample_Li(const AInteraction &ref, const AVector2f &u,
			AVector3f &wi, Float &pdf, AVisibilityTester &vis) const = 0;

		virtual Float pdf_Li(const AInteraction &, const AVector3f &) const = 0;

		//Emission
		virtual ASpectrum Le(const ARay &r) const;

		virtual ASpectrum sample_Le(const AVector2f &u1, const AVector2f &u2, ARay &ray,
			AVector3f &nLight, Float &pdfPos, Float &pdfDir) const = 0;

		virtual void pdf_Le(const ARay &, const AVector3f &, Float &pdfPos, Float &pdfDir) const = 0;

		virtual AClassType getClassType() const override { return AClassType::AELight; }

		int m_flags;
		int m_nSamples;

	protected:
		ATransform m_lightToWorld, m_worldToLight;
	};

	class AVisibilityTester final
	{
	public:
		AVisibilityTester() {}
		AVisibilityTester(const AInteraction &p0, const AInteraction &p1)
			: m_p0(p0), m_p1(p1) {}

		const AInteraction &P0() const { return m_p0; }
		const AInteraction &P1() const { return m_p1; }

		bool unoccluded(const AScene &scene) const;

		ASpectrum tr(const AScene &scene, ASampler &sampler) const;

	private:
		AInteraction m_p0, m_p1;
	};

	class AAreaLight : public ALight
	{
	public:
		typedef std::shared_ptr<AAreaLight> ptr;

		AAreaLight(const APropertyList &props);
		AAreaLight(const ATransform &lightToWorld, int nSamples);
		virtual ASpectrum L(const AInteraction &intr, const AVector3f &w) const = 0;
	};

}

#endif