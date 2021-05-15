#ifndef ARSHAPE_H
#define ARSHAPE_H

#include "ArAurora.h"
#include "ArMathUtils.h"
#include "ArTransform.h"
#include "ArRtti.h"

#include <vector>

namespace Aurora
{
	class AShape : public AObject
	{
	public:
		typedef std::shared_ptr<AShape> ptr;

		AShape(const APropertyList &props);
		AShape(ATransform *objectToWorld, ATransform *worldToObject);
		virtual ~AShape() = default;

		void setTransform(ATransform *objectToWorld, ATransform *worldToObject);

		virtual ABounds3f objectBound() const = 0;
		virtual ABounds3f worldBound() const;

		virtual bool hit(const ARay &ray) const;
		virtual bool hit(const ARay &ray, Float &tHit, ASurfaceInteraction &isect) const = 0;

		virtual Float area() const = 0;

		// Sample a point on the surface of the shape and return the PDF with
		// respect to area on the surface.
		virtual AInteraction sample(const AVector2f &u, Float &pdf) const = 0;
		virtual Float pdf(const AInteraction &) const { return 1 / area(); }

		// Sample a point on the shape given a reference point |ref| and
		// return the PDF with respect to solid angle from |ref|.
		virtual AInteraction sample(const AInteraction &ref, const AVector2f &u, Float &pdf) const;
		virtual Float pdf(const AInteraction &ref, const AVector3f &wi) const;

		// Returns the solid angle subtended by the shape w.r.t. the reference
		// point p, given in world space. Some shapes compute this value in
		// closed-form, while the default implementation uses Monte Carlo
		// integration; the nSamples parameter determines how many samples are
		// used in this case.
		virtual Float solidAngle(const AVector3f &p, int nSamples = 512) const;

		virtual AClassType getClassType() const override { return AClassType::AEShape; }

		ATransform *m_objectToWorld = nullptr, *m_worldToObject = nullptr;
	};

}

#endif