#ifndef AABB_H
#define AABB_H

#include <vector>

#include "Ray.h"
#include "Vector3D.h"

/**
 * @projectName   RayTracer
 * @brief         AABB bounding box.
 * @author        YangWC
 * @date          2019-05-10
 */
namespace RayTracer
{

	inline float ffmin(const float &a, const float &b) { return a < b ? a : b; }
	inline float ffmax(const float &a, const float &b) { return a > b ? a : b; }

	class AABB
	{
	private:
		Vector3D m_min, m_max;

	public:
		AABB() = default;
		AABB(const Vector3D &a, const Vector3D &b) { m_min = a; m_max = b; }

		Vector3D getMin() const { return m_min; }
		Vector3D getMax() const { return m_max; }

		bool isInside(const Vector3D &point) const
		{
			return (
				(point.x <= m_max.x && point.x >= m_min.x) &&
				(point.y <= m_max.y && point.y >= m_min.y) &&
				(point.z <= m_max.z && point.z >= m_min.z));
		}

		std::vector<AABB> getEightSubAABB() const;

		bool hit(const Ray &ray, float tmin, float tmax) const
		{
			//£¡whether the ray intersects with the box or not.
			float t0, t1, invD;

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
			Vector3D small(fmin(box0.getMin().x, box1.getMin().x),
				fmin(box0.getMin().y, box1.getMin().y),
				fmin(box0.getMin().z, box1.getMin().z));
			Vector3D big(fmax(box0.getMax().x, box1.getMax().x),
				fmax(box0.getMax().y, box1.getMax().y),
				fmax(box0.getMax().z, box1.getMax().z));
			return AABB(small, big);
		}
	};

}

#endif // AABB_H
