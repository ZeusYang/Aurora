#ifndef ARBOXFILTER_H
#define ARBOXFILTER_H

#include "ArFilter.h"

namespace Aurora
{
	class ABoxFilter final : public AFilter
	{
	public:

		ABoxFilter(const APropertyTreeNode &node);
		ABoxFilter(const AVector2f &radius) : AFilter(radius) {}

		virtual Float evaluate(const AVector2f &p) const override;

		virtual std::string toString() const override { return "BoxFilter[]"; }

	};
}

#endif