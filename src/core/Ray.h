#ifndef RAY_H
#define RAY_H

#include "ArMathUtils.h"


namespace Aurora
{

	class Ray
	{
	private:
		AVector3f m_origin;
		AVector3f m_direction;

	public:
		// ctor/dtor.
		Ray() = default;
		~Ray() = default;
		Ray(const AVector3f &org, const AVector3f &dir)
			:m_origin(org), m_direction(normalize(dir))
		{
		}

		// Getter.
		AVector3f getOrigin() const { return m_origin; }
		AVector3f getDirection() const { return m_direction; }

		// p(t) = origin + t*dir;
		AVector3f pointAt(const Float &t)const { return m_origin + m_direction * t; }
	};

}

#endif // RAY_H
