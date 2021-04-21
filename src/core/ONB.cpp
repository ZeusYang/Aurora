#include "ONB.h"

namespace RayTracer
{

	void ONB::buildFromW(const Vector3D &n)
	{
		m_axis[2] = n;
		m_axis[2].normalize();

		Vector3D a;
		if (fabs(w().x) > 0.9f)
			a = Vector3D(0, 1, 0);
		else
			a = Vector3D(1, 0, 0);
		m_axis[1] = w().crossProduct(a);
		m_axis[1].normalize();

		m_axis[0] = w().crossProduct(v());
		m_axis[0].normalize();
	}

}
