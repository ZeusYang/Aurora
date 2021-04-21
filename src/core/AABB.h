#ifndef AABB_H
#define AABB_H

#include <vector>

#include "Ray.h"
#include "ArMathUtils.h"

namespace Aurora
{

	inline Float ffmin(const Float &a, const Float &b) { return a < b ? a : b; }
	inline Float ffmax(const Float &a, const Float &b) { return a > b ? a : b; }

	class AABB
	{
	private:
		AVector3f m_min, m_max;

	public:
		AABB() = default;
		AABB(const AVector3f &a, const AVector3f &b) { m_min = a; m_max = b; }

		AVector3f getMin() const { return m_min; }
		AVector3f getMax() const { return m_max; }

		bool isInside(const AVector3f &point) const
		{
			return (
				(point.x <= m_max.x && point.x >= m_min.x) &&
				(point.y <= m_max.y && point.y >= m_min.y) &&
				(point.z <= m_max.z && point.z >= m_min.z));
		}

		std::vector<AABB> getEightSubAABB() const;

		bool hit(const Ray &ray, Float tmin, Float tmax) const
		{
			//£¡whether the ray intersects with the box or not.
			Float t0, t1, invD;

			// x
			invD = 1.0f / ray.getDirection().x;
			t0 = (m_min.x - ray.getOrigin().x) * invD;
			t1 = (m_max.x - ray.getOrigin().x) * invD;
			if (invD < 0.0f)
				std::swap(t0, t1);
			tmin = t0 > tmin ? t0 : tmin;
			tmax = t1 < tmax ? t1 : tmax;
			if (tmax <= tmin) return false;

			// y
			invD = 1.0f / ray.getDirection().y;
			t0 = (m_min.y - ray.getOrigin().y) * invD;
			t1 = (m_max.y - ray.getOrigin().y) * invD;
			if (invD < 0.0f)
				std::swap(t0, t1);
			tmin = t0 > tmin ? t0 : tmin;
			tmax = t1 < tmax ? t1 : tmax;
			if (tmax <= tmin) return false;

			// z
			invD = 1.0f / ray.getDirection().z;
			t0 = (m_min.z - ray.getOrigin().z) * invD;
			t1 = (m_max.z - ray.getOrigin().z) * invD;
			if (invD < 0.0f)
				std::swap(t0, t1);
			tmin = t0 > tmin ? t0 : tmin;
			tmax = t1 < tmax ? t1 : tmax;
			if (tmax <= tmin) return false;

			return true;
		}

		static AABB surroundingBox(const AABB &box0, const AABB &box1)
		{
			AVector3f small(fmin(box0.getMin().x, box1.getMin().x),
				fmin(box0.getMin().y, box1.getMin().y),
				fmin(box0.getMin().z, box1.getMin().z));
			AVector3f big(fmax(box0.getMax().x, box1.getMax().x),
				fmax(box0.getMax().y, box1.getMax().y),
				fmax(box0.getMax().z, box1.getMax().z));
			return AABB(small, big);
		}
	};

}

#endif // AABB_H
