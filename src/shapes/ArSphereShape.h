#ifndef ARSPHERERSHAPE_H
#define ARSPHERERSHAPE_H

#include "ArShape.h"

namespace Aurora
{
	class ASphereShape final : public AShape
	{
	public:
		typedef std::shared_ptr<ASphereShape> ptr;

		ASphereShape(const APropertyTreeNode &node);
		ASphereShape(ATransform *objectToWorld, ATransform *worldToObject, const float radius);

		virtual ~ASphereShape() = default;

		virtual Float area() const override;

		virtual AInteraction sample(const AVector2f &u, Float &pdf) const override;

		virtual AInteraction sample(const AInteraction &ref, const AVector2f &u, Float &pdf) const override;
		virtual Float pdf(const AInteraction &ref, const AVector3f &wi) const override;

		virtual ABounds3f objectBound() const override;

		virtual bool hit(const ARay &ray) const override;
		virtual bool hit(const ARay &ray, Float &tHit, ASurfaceInteraction &isect) const override;

		virtual Float solidAngle(const AVector3f &p, int nSamples = 512) const override;

		virtual std::string toString() const override { return "SphereShape[]"; }

	private:
		float m_radius;
	};
}

#endif