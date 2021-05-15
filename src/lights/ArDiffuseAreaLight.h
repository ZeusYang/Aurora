#ifndef ARDIFFUSE_AREA_LIGHT_H
#define ARDIFFUSE_AREA_LIGHT_H

#include "ArLight.h"

namespace Aurora
{
	class ADiffuseAreaLight final : public AAreaLight
	{
	public:
		typedef std::shared_ptr<ADiffuseAreaLight> ptr;

		ADiffuseAreaLight(const APropertyTreeNode &node);

		ADiffuseAreaLight(const ATransform &lightToWorld, const ASpectrum &Le, int nSamples,
			AShape* shape, bool twoSided = false);

		virtual ASpectrum L(const AInteraction &intr, const AVector3f &w) const override
		{
			return (m_twoSided || dot(intr.n, w) > 0) ? m_Lemit : ASpectrum(0.f);
		}

		virtual ASpectrum power() const override;

		virtual ASpectrum sample_Li(const AInteraction &ref, const AVector2f &u, AVector3f &wo,
			Float &pdf, AVisibilityTester &vis) const override;

		virtual Float pdf_Li(const AInteraction &, const AVector3f &) const override;

		virtual ASpectrum sample_Le(const AVector2f &u1, const AVector2f &u2, ARay &ray,
			AVector3f &nLight, Float &pdfPos, Float &pdfDir) const override;

		virtual void pdf_Le(const ARay &, const AVector3f &, Float &pdfPos, Float &pdfDir) const override;

		virtual std::string toString() const override { return "DiffuseAreaLight[]"; }

		virtual void setParent(AObject *parent) override;

	protected:

		ASpectrum m_Lemit;
		AShape* m_shape;
		// Added after book publication: by default, DiffuseAreaLights still
		// only emit in the hemimsphere around the surface normal.  However,
		// this behavior can now be overridden to give emission on both sides.
		bool m_twoSided;
		Float m_area;
	};
}

#endif