#include "Hitable.h"

#include "ONB.h"

namespace Aurora
{

	bool Sphere::hit(const ARay &ray, HitRecord &ret) const
	{
		AVector3f oc = ray.origin() - m_center;
		Float a = dot(ray.direction(), ray.direction());
		Float b = dot(oc, ray.direction());
		Float c = dot(oc, oc) - m_radius * m_radius;
		// discriminant
		Float discriminant = b * b - a * c;
		if (discriminant > 0)
		{
			Float temp = (-b - glm::sqrt(discriminant)) / a;
			if (temp > 0.0f && temp <= ray.m_tMax)
			{
				ret.m_t = temp;
				ray.m_tMax = temp;
				ret.m_position = ray(ret.m_t);
				//AVector3f::getSphereUV((ret.m_position - m_center) / m_radius, ret.m_texcoord);
				ret.m_texcoord = AVector2f(0.0f);
				ret.m_normal = (ret.m_position - m_center) / m_radius;
				ret.m_material = m_material.get();
				return true;
			}
			temp = (-b + glm::sqrt(discriminant)) / a;
			if (temp > 0.0f && temp <= ray.m_tMax)
			{
				ret.m_t = temp;
				ray.m_tMax = temp;
				ret.m_position = ray(ret.m_t);
				//AVector3f::getSphereUV((ret.m_position - m_center) / m_radius, ret.m_texcoord);
				ret.m_texcoord = AVector2f(0.0f);
				ret.m_normal = (ret.m_position - m_center) / m_radius;
				ret.m_material = m_material.get();
				return true;
			}
		}
		return false;
	}

	Float Sphere::pdfValue(const AVector3f &o, const AVector3f &v) const
	{
		HitRecord rec;
		if (this->hit(ARay(o, v), rec))
		{
			Float cos_theta_max = glm::sqrt(1 - m_radius * m_radius / lengthSquared(m_center - o));
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

	bool Triangle::hit(const ARay &ray, HitRecord &ret) const
	{
		//Float n_dot_dir = dot(m_normal, ray.direction());
		//// no intersection.
		//if (equal(n_dot_dir, 0.0))
		//	return false;
		//Float d = dot(-m_normal, m_p0);
		//Float t = -(dot(m_normal, ray.origin()) + d) / n_dot_dir;
		//if (t <= 0.0f || t > ray.m_tMax)
		//	return false;
		//ret.m_t = t;
		//ret.m_position = ray(t);
		//ret.m_material = m_material.get();
		//// judge inside or not.
		//AVector3f r = ret.m_position - m_p0;
		//AVector3f q1 = m_p1 - m_p0;
		//AVector3f q2 = m_p2 - m_p0;
		//Float q1_squaredLen = lengthSquared(q1);
		//Float q2_squaredLen = lengthSquared(q2);
		//Float q1_dot_q2 = dot(q1, q2);
		//Float r_dot_q1 = dot(r, q1);
		//Float r_dot_q2 = dot(r, q2);
		//Float determinant = 1.0f / (q1_squaredLen * q2_squaredLen - q1_dot_q2 * q1_dot_q2);

		//Float omega1 = determinant * (q2_squaredLen * r_dot_q1 - q1_dot_q2 * r_dot_q2);
		//Float omega2 = determinant * (-q1_dot_q2 * r_dot_q1 + q1_squaredLen * r_dot_q2);
		//if (omega1 + omega2 > 1.0f || omega1 < 0.0f || omega2 < 0.0f)
		//	return false;

		//ray.m_tMax = t;

		AVector3f E1 = m_p1 - m_p0;
		AVector3f E2 = m_p2 - m_p0;

		AVector3f P = cross(ray.direction(), E2);
		Float det = dot(E1, P);

		AVector3f T;
		if (det > 0)
		{
			T = ray.origin() - m_p0;
		}
		else
		{
			T = m_p0 - ray.origin();
			det = -det;
		}

		if (det < 0.0001f)
			return false;

		Float t, u, v;

		u = dot(T, P);
		if (u < 0.0f || u > det)
			return false;

		AVector3f Q = cross(T, E1);

		v = dot(ray.direction(), Q);
		if (v < 0.0f || (u + v)>det)
			return false;

		t = dot(E2, Q);
		Float fInvDet = 1.0f / det;

		t *= fInvDet;
		//u *= fInvDet;
		//v *= fInvDet;

		if (t <= 0.0f || t > ray.m_tMax)
			return false;

		ray.m_tMax = t;
		ret.m_t = t;
		ret.m_position = ray(t);
		ret.m_material = m_material.get();
		ret.m_texcoord = AVector2f(0.0f);
		ret.m_normal = m_normal;

		return true;
	}

	bool HitableList::hit(const ARay &ray, HitRecord &ret) const
	{
		HitRecord temp_rec;
		bool hit_anything = false;
		//Float closest_so_far = ray.m_tMax;
		for (int i = 0; i < m_objects.size(); i++)
		{
			if (m_objects[i]->hit(ray, temp_rec))
			{
				hit_anything = true;
				//closest_so_far = temp_rec.m_t;
				ret = temp_rec;
			}
		}
		return hit_anything;
	}

	Float HitableList::pdfValue(const AVector3f &o, const AVector3f &v) const
	{
		Float weight = 1.0f / m_objects.size();
		Float sum = 0;
		for (int x = 0; x < m_objects.size(); ++x)
			sum += m_objects[x]->pdfValue(o, v);
		return sum * weight;
	}

	AVector3f HitableList::random(const AVector3f &o) const
	{
		int index = static_cast<int>(drand48() * m_objects.size());
		return m_objects[index]->random(o);
	}

}
