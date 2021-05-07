#include "ArFilter.h"

namespace Aurora
{
	AURORA_REGISTER_CLASS(ABoxFilter, "Box")

	AFilter::AFilter(const APropertyList &props) : 
		m_radius(props.getVector2f("Radius", AVector2f(0.5f))),
		m_invRadius(AVector2f(1/m_radius.x, 1/m_radius.y)) {}

	ABoxFilter::ABoxFilter(const APropertyTreeNode &node) : AFilter(node.getPropertyList()) { activate(); }

	Float ABoxFilter::evaluate(const AVector2f &p) const
	{
		return 1.0f;
	}
}