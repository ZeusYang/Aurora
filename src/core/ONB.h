#ifndef ONB_H
#define ONB_H

#include "Vector3D.h"

/**
 * @projectName   RayTracer
 * @brief         Ortho normal basis.
 * @author        YangWC
 * @date          2019-05-18
 */

namespace RayTracer
{

	class ONB
	{
	private:
		Vector3D m_axis[3];

	public:
		ONB() = default;
		Vector3D u() const { return m_axis[0]; }
		Vector3D v() const { return m_axis[1]; }
		Vector3D w() const { return m_axis[2]; }

		Vector3D operator[](int i) const { return m_axis[i]; }

		Vector3D local(float a, float b, float c) const
		{
			return u() * a + v() * b + w() * c;
		}

		Vector3D local(const Vector3D &a) const
		{
			return u() * a.x + v() * a.y + w() * a.z;
		}

		void buildFromW(const Vector3D &n);

	};

}

#endif // ONB_H
