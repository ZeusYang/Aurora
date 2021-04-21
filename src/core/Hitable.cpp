#include "Hitable.h"

#include "ONB.h"

namespace RayTracer
{

	bool Sphere::hit(const Ray &ray, const float &t_min, const float &t_max, HitRecord &ret) const
	{
		Vector3D oc = ray.getOrigin() - m_center;
		float a = ray.getDirection().dotProduct(ray.getDirection());
		float b = oc.dotProduct(ray.getDirection());
		float c = oc.dotProduct(oc) - m_radius * m_radius;
		// discriminant
		float discriminant = b * b - a * c;
		if (discriminant > 0)
		{
			float temp = (-b - sqrt(discriminant)) / a;
			if (temp > t_min && temp < t_max)
			{
				ret.m_t = temp;
				ret.m_position = ray.pointAt(ret.m_t);
				Vector3D::getSphereUV((ret.m_position - m_center) / m_radius, ret.m_texcoord);
				ret.m_normal = (ret.m_position - m_center) / m_radius;
				ret.m_material = m_material;
				return true;
			}
			temp = (-b + sqrt(discriminant)) / a;
			if (temp > t_min && temp < t_max)
			{
				ret.m_t = temp;
				ret.m_position = ray.pointAt(ret.m_t);
				Vector3D::getSphereUV((ret.m_position - m_center) / m_radius, ret.m_texcoord);
				ret.m_normal = (ret.m_position - m_center) / m_radius;
				ret.m_material = m_material;
				return true;
			}
		}
		return false;
	}

	bool Sphere::boundingBox(const float &t0, const float &t1, AABB &box) const
	{
		box = AABB(m_center - Vector3D(fabs(m_radius), fabs(m_radius), fabs(m_radius)),
			m_center + Vector3D(fabs(m_radius), fabs(m_radius), fabs(m_radius)));
		return true;
	}

	float Sphere::pdfValue(const Vector3D &o, const Vector3D &v) const
	{
		HitRecord rec;
		if (this->hit(Ray(o, v), 0.001f, FLT_MAX, rec))
		{
			float cos_theta_max = sqrt(1 - m_radius * m_radius / (m_center - o).getSquaredLength());
			float solid_angle = 2 * M_PI * (1 - cos_theta_max);
			return 1.0f / solid_angle;
		}
		else
			return 0.0f;
	}

	Vector3D Sphere::random(const Vector3D &o) const
	{
		Vector3D dir = m_center - o;
		float distance_squared = dir.getSquaredLength();
		ONB uvw;
		uvw.buildFromW(dir);
		return uvw.local(Vector3D::randomToSphere(m_radius, distance_squared));
	}

	bool TTriangle::hit(const Ray &ray, const float &t_min, const float &t_max, HitRecord &ret) const
	{
		float n_dot_dir = m_normal.dotProduct(ray.getDirection());
		// no intersection.
		if (equal(n_dot_dir, 0.0))
			return false;
		float d = -m_normal.dotProduct(m_p0);
		float t = -(m_normal.dotProduct(ray.getOrigin()) + d) / n_dot_dir;
		if (t < t_min || t > t_max)
			return false;
		ret.m_t = t;
		ret.m_position = ray.pointAt(t);
		ret.m_material = m_material;
		// judge inside or not.
		Vector3D r = ret.m_position - m_p0;
		Vector3D q1 = m_p1 - m_p0;
		Vector3D q2 = m_p2 - m_p0;
		float q1_squaredLen = q1.getSquaredLength();
		float q2_squaredLen = q2.getSquaredLength();
		float q1_dot_q2 = q1.dotProduct(q2);
		float r_dot_q1 = r.dotProduct(q1);
		float r_dot_q2 = r.dotProduct(q2);
		float determinant = 1.0f / (q1_squaredLen * q2_squaredLen - q1_dot_q2 * q1_dot_q2);

		float omega1 = determinant * (q2_squaredLen * r_dot_q1 - q1_dot_q2 * r_dot_q2);
		float omega2 = determinant * (-q1_dot_q2 * r_dot_q1 + q1_squaredLen * r_dot_q2);
		if (omega1 + omega2 > 1.0f || omega1 < 0.0f || omega2 < 0.0f)
			return false;
		return true;
	}

	bool TTriangle::boundingBox(const float &t0, const float &t1, AABB &box) const
	{
		Vector3D minp, maxp;
		minp.x = fmin(m_p0.x, fmin(m_p1.x, m_p2.x));
		minp.y = fmin(m_p0.y, fmin(m_p1.y, m_p2.y));
		minp.z = fmin(m_p0.z, fmin(m_p1.z, m_p2.z));
		maxp.x = fmax(m_p0.x, fmax(m_p1.x, m_p2.x));
		maxp.y = fmax(m_p0.y, fmax(m_p1.y, m_p2.y));
		maxp.z = fmax(m_p0.z, fmax(m_p1.z, m_p2.z));
		box = AABB(minp, maxp);
		return true;
	}

	bool HitableList::hit(const Ray &ray, const float &t_min, const float &t_max, HitRecord &ret) const
	{
		HitRecord temp_rec;
		bool hit_anything = false;
		double closest_so_far = t_max;
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

	bool HitableList::boundingBox(const float &t0, const float &t1, AABB &box) const
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

	float HitableList::pdfValue(const Vector3D &o, const Vector3D &v) const
	{
		float weight = 1.0f / m_list.size();
		float sum = 0;
		for (int x = 0; x < m_list.size(); ++x)
			sum += m_list[x]->pdfValue(o, v);
		return sum * weight;
	}

	Vector3D HitableList::random(const Vector3D &o) const
	{
		int index = static_cast<int>(drand48() * m_list.size());
		return m_list[index]->random(o);
	}

}
