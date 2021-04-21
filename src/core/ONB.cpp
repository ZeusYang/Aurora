#include "ONB.h"

namespace Aurora
{

	void ONB::buildFromW(const AVector3f &n)
	{
		m_axis[2] = normalize(n);

		AVector3f a;
		if (fabs(w().x) > 0.9f)
			a = AVector3f(0, 1, 0);
		else
			a = AVector3f(1, 0, 0);
		m_axis[1] = normalize(cross(w(), a));

		m_axis[0] = normalize(cross(w(), v()));
	}

}
