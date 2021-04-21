#include "Hitable.h"

#include "ONB.h"

namespace Aurora
{

	bool Sphere::hit(const Ray &ray, const Float &t_min, const Float &t_max, HitRecord &ret) const
	{
		AVector3f oc = ray.getOrigin() - m_center;
		Float a = dot(ray.getDirection(), ray.getDirection());
		Float b = dot(oc, ray.getDirection());
		Float c = dot(oc, oc) - m_radius * m_radius;
		// discriminant
		Float discriminant = b * b - a * c;
		if (discriminant > 0)
		{
			Float temp = (-b - sqrt(discriminant)) / a;
			if (temp > t_min && temp < t_max)
			{
				ret.m_t = temp;
				ret.m_position = ray.pointAt(ret.m_t);
				//AVector3f::getSphereUV((ret.m_position - m_center) / m_radius, ret.m_texcoord);
				ret.m_texcoord = AVector2f(0.0f);
				ret.m_normal = (ret.m_position - m_center) / m_radius;
				ret.m_material = m_material.get();
				return true;
			}
			temp = (-b + sqrt(discriminant)) / a;
			if (temp > t_min && temp < t_max)
			{
				ret.m_t = temp;
				ret.m_position = ray.pointAt(ret.m_t);
				//AVector3f::getSphereUV((ret.m_position - m_center) / m_radius, ret.m_texcoord);
				ret.m_texcoord = AVector2f(0.0f);
				ret.m_normal = (ret.m_position - m_center) / m_radius;
				ret.m_material = m_material.get();
				return true;
			}
		}
		return false;
	}

	bool Sphere::boundingBox(const Float &t0, const Float &t1, AABB &box) const
	{
		box = AABB(m_center - AVector3f(fabs(m_radius), fabs(m_radius), fabs(m_radius)),
			m_center + AVector3f(fabs(m_radius), fabs(m_radius), fabs(m_radius)));
		return true;
	}

	Float Sphere::pdfValue(const AVector3f &o, const AVector3f &v) const
	{
		HitRecord rec;
		if (this->hit(Ray(o, v), 0.001f, FLT_MAX, rec))
		{
			Float cos_theta_max = sqrt(1 - m_radius * m_radius / lengthSquared(m_center - o));
			Float solid_angle = 2 * aPi * (1 - cos_theta_max);
			return 1.0f / solid_angle;
		}
		else
			return 0.0f;
	}

	AVector3f Sphere::random(const AVector3f &o) const
	{
		AVector3f dir = m_center - o;
		Float distance_squared = lengthSquared(dir);
		ONB uvw;
		uvw.buildFromW(dir);
		return uvw.local(randomToSphere(m_radius, distance_squared));
	}

	bool TTriangle::hit(const Ray &ray, const Float &t_min, const Float &t_max, HitRecord &ret) const
	{
		Float n_dot_dir = dot(m_normal, ray.getDirection());
		// no intersection.
		if (equal(n_dot_dir, 0.0))
			return false;
		Float d = dot(-m_normal, m_p0);
		Float t = -(dot(m_normal, ray.getOrigin()) + d) / n_dot_dir;
		if (t < t_min || t > t_max)
			return false;
		ret.m_t = t;
		ret.m_position = ray.pointAt(t);
		ret.m_material = m_material.get();
		// judge inside or not.
		AVector3f r = ret.m_position - m_p0;
		AVector3f q1 = m_p1 - m_p0;
		AVector3f q2 = m_p2 - m_p0;
		Float q1_squaredLen = lengthSquared(q1);
		Float q2_squaredLen = lengthSquared(q2);
		Float q1_dot_q2 = dot(q1, q2);
		Float r_dot_q1 = dot(r, q1);
		Float r_dot_q2 = dot(r, q2);
		Float determinant = 1.0f / (q1_squaredLen * q2_squaredLen - q1_dot_q2 * q1_dot_q2);

		Float omega1 = determinant * (q2_squaredLen * r_dot_q1 - q1_dot_q2 * r_dot_q2);
		Float omega2 = determinant * (-q1_dot_q2 * r_dot_q1 + q1_squaredLen * r_dot_q2);
		if (omega1 + omega2 > 1.0f || omega1 < 0.0f || omega2 < 0.0f)
			return false;
		return true;
	}

	bool TTriangle::boundingBox(const Float &t0, const Float &t1, AABB &box) const
	{
		AVector3f minp, maxp;
		minp.x = fmin(m_p0.x, fmin(m_p1.x, m_p2.x));
		minp.y = fmin(m_p0.y, fmin(m_p1.y, m_p2.y));
		minp.z = fmin(m_p0.z, fmin(m_p1.z, m_p2.z));
		maxp.x = fmax(m_p0.x, fmax(m_p1.x, m_p2.x));
		maxp.y = fmax(m_p0.y, fmax(m_p1.y, m_p2.y));
		maxp.z = fmax(m_p0.z, fmax(m_p1.z, m_p2.z));
		box = AABB(minp, maxp);
		return true;
	}

	bool HitableList::hit(const Ray &ray, const Float &t_min, const Float &t_max, HitRecord &ret) const
	{
		HitRecord temp_rec;
		bool hit_anything = false;
		Float closest_so_far = t_max;
		for (int i = 0; i < m_list.size(); i++) {
			if (m_list[i]->hit(ray, t_min, closest_so_far, temp_rec))
			{
				hit_anything = true;
				closest_so_far = temp_rec.m_t;
				ret = temp_rec;
			}
		}
		return hit_anything;
	}

	bool HitableList::boundingBox(const Float &t0, const Float &t1, AABB &box) const
	{
		if (m_list.size() < 1) return false;
		AABB temp_box;
		bool first_true = m_list[0]->boundingBox(t0, t1, temp_box);
		if (!first_true)
			return false;
		else
			box = temp_box;
		for (int i = 1; i < m_list.size(); i++)
		{
			if (m_list[0]->boundingBox(t0, t1, temp_box))
				box = AABB::surroundingBox(box, temp_box);
			else
				return false;
		}
		return true;
	}

	Float HitableList::pdfValue(const AVector3f &o, const AVector3f &v) const
	{
		Float weight = 1.0f / m_list.size();
		Float sum = 0;
		for (int x = 0; x < m_list.size(); ++x)
			sum += m_list[x]->pdfValue(o, v);
		return sum * weight;
	}

	AVector3f HitableList::random(const AVector3f &o) const
	{
		int index = static_cast<int>(drand48() * m_list.size());
		return m_list[index]->random(o);
	}

}
