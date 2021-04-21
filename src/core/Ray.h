#ifndef RAY_H
#define RAY_H

#include "Vector3D.h"

/**
 * @projectName   RayTracer
 * @brief         A ray representation class.
 * @author        YangWC
 * @date          2019-05-06
 */

namespace RayTracer
{

	class Ray
	{
	private:
		Vector3D m_origin;
		Vector3D m_direction;

	public:
		// ctor/dtor.
		Ray() = default;
		~Ray() = default;
		Ray(const Vector3D &org, const Vector3D &dir)
			:m_origin(org), m_direction(dir)
		{
			m_direction.normalize();
		}

		// Getter.
		Vector3D getOrigin() const { return m_origin; }
		Vector3D getDirection() const { return m_direction; }

		// p(t) = origin + t*dir;
		Vector3D pointAt(const float &t)const { return m_origin + m_direction * t; }
	};

}

#endif // RAY_H
