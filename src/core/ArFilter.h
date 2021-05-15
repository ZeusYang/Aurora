#ifndef ARFILTER_H
#define ARFILTER_H

#include "ArAurora.h"
#include "ArMathUtils.h"
#include "ArRtti.h"

namespace Aurora
{
	class AFilter : public AObject
	{
	public:
		
		virtual ~AFilter() = default;

		AFilter(const APropertyList &props);
		AFilter(const AVector2f &radius)
			: m_radius(radius), m_invRadius(AVector2f(1 / radius.x, 1 / radius.y)) {}

		virtual Float evaluate(const AVector2f &p) const = 0;

		virtual AClassType getClassType() const override { return AClassType::AEFilter; }

		const AVector2f m_radius, m_invRadius;
	};

}

#endif