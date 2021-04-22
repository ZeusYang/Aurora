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
		for (int i = 0; i < m_objects.size(); i++)
		{
			if (m_objects[i]->hit(ray, temp_rec))
			{
				hit_anything = true;
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
