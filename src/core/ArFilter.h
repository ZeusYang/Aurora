#ifndef ARFILTER_H
#define ARFILTER_H

#include "ArAurora.h"
#include "ArMathUtils.h"

namespace Aurora
{
	class AFilter 
	{
	public:
		
		virtual ~AFilter() = default;

		AFilter(const AVector2f &radius)
			: m_radius(radius), m_invRadius(AVector2f(1 / radius.x, 1 / radius.y)) {}

		virtual Float evaluate(const AVector2f &p) const = 0;


		const AVector2f m_radius, m_invRadius;
	};

	class ABoxFilter final : public AFilter
	{
	public:
		ABoxFilter(const AVector2f &radius) : AFilter(radius) {}

		virtual Float evaluate(const AVector2f &p) const override;
	};
}

#endif