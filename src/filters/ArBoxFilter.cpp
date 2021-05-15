#include "ArBoxFilter.h"

namespace Aurora
{
	AURORA_REGISTER_CLASS(ABoxFilter, "Box")

	ABoxFilter::ABoxFilter(const APropertyTreeNode &node) : AFilter(node.getPropertyList()) 
	{ 
		activate(); 
	}

	Float ABoxFilter::evaluate(const AVector2f &p) const
	{
		return 1.0f;
	}
}