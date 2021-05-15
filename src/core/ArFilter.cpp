#include "ArFilter.h"

namespace Aurora
{
	AFilter::AFilter(const APropertyList &props) :
		m_radius(props.getVector2f("Radius", AVector2f(0.5f))),
		m_invRadius(AVector2f(1 / m_radius.x, 1 / m_radius.y)) {}
}