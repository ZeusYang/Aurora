#include "MeshHitable.h"

namespace RayTracer
{

	void MeshHitable::preRendering()
	{
		// transform and calculate aabb box.
		if (!m_transformation.getDirtry() || m_indices.empty())
			return;
		Vector3D minPoint(+FLT_MAX, +FLT_MAX, +FLT_MAX);
		Vector3D maxPoint(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		Matrix4x4 modelMatrix = m_transformation.toMatrix();
		Matrix4x4 invModelMatrix = m_transformation.toInvMatrix();
		Vector4D pos, nor;
		unsigned int p0, p1, p2;
		for (int x = 0; x < m_vertices.size(); ++x)
		{
			int index = x;
			pos = m_vertices[index].m_position;
			nor = m_vertices[index].m_normal;
			pos.w = 1.0f;
			nor.w = 0.0f;
			pos = modelMatrix * pos;
			nor = invModelMatrix * nor;
			m_vertices[index].m_position = pos;
			m_vertices[index].m_normal = nor;
			m_vertices[index].m_normal.normalize();
			minPoint.x = fmin(minPoint.x, pos.x);
			minPoint.y = fmin(minPoint.y, pos.y);
			minPoint.z = fmin(minPoint.z, pos.z);
			maxPoint.x = fmax(maxPoint.x, pos.x);
			maxPoint.y = fmax(maxPoint.y, pos.y);
			maxPoint.z = fmax(maxPoint.z, pos.z);
		}
		if (equal(minPoint.x, maxPoint.x))
		{
			minPoint.x -= 0.0001f;
			maxPoint.x += 0.0001f;
		}
		if (equal(minPoint.y, maxPoint.y))
		{
			minPoint.y -= 0.0001f;
			maxPoint.y += 0.0001f;
		}
		if (equal(minPoint.z, maxPoint.z))
		{
			minPoint.z -= 0.0001f;
			maxPoint.z += 0.0001f;
		}

		// face normal
		for (int x = 0; x < m_indices.size(); x += 3)
		{
			int index0 = m_indices[x + 0];
			int index1 = m_indices[x + 1];
			int index2 = m_indices[x + 2];
			Vector3D normal = (m_vertices[index1].m_position - m_vertices[index0].m_position)
				.crossProduct(m_vertices[index2].m_position - m_vertices[index0].m_position);
			normal.normalize();
			m_faceNormal.push_back(normal);
		}
		m_box = AABB(minPoint, maxPoint);

		//! if the number of triangles is more than a threshold, build a octree for 
		if (m_indices.size() > 100 * 3)
		{
			m_octree = std::shared_ptr<Octree>(new Octree(minPoint, maxPoint, 10));
			m_octree->build(this);
		}
		else
			m_octree = nullptr;
	}

	bool MeshHitable::hit(const Ray &ray, const float &t_min, const float &t_max, HitRecord &ret) const
	{
		HitRecord tmpRec;
		bool hitAny = false;

		//! octree for accelerating intersection detection.
		if (m_octree != nullptr)
		{
			m_octree->visit(ray, t_min, t_max, 
				[&](unsigned int i1, unsigned int i2, unsigned int i3, float &tMax) -> bool
			{
				Vector3D normal = (m_vertices[i2].m_position - m_vertices[i1].m_position)
					.crossProduct(m_vertices[i3].m_position - m_vertices[i1].m_position);
				normal.normalize();
				if (triangleHit(ray, t_min, tMax, tmpRec,
					m_vertices[i1],
					m_vertices[i2],
					m_vertices[i3],
					normal))
				{
					hitAny = true;
					tMax = tmpRec.m_t;
					ret = tmpRec;
					return true;
				}
				return false;
			});
		}
		else
		{
			//! brute-force algorithm.
			float closestSoFar = t_max;
			for (int x = 0; x < m_indices.size(); x += 3)
			{
				int index1 = m_indices[x + 0];
				int index2 = m_indices[x + 1];
				int index3 = m_indices[x + 2];
				if (triangleHit(ray, t_min, closestSoFar, tmpRec,
					m_vertices[index1],
					m_vertices[index2],
					m_vertices[index3],
					m_faceNormal[x / 3]))
				{
					hitAny = true;
					closestSoFar = tmpRec.m_t;
					ret = tmpRec;
				}
			}
		}
		return hitAny;
	}

	bool MeshHitable::boundingBox(const float &t0, const float &t1, AABB &box) const
	{
		(void)t0;
		(void)t1;
		box = m_box;
		return true;
	}

	bool MeshHitable::triangleHit(const Ray &ray, const float &t_min, const float &t_max,
		HitRecord &ret, const Vertex &p0, const Vertex &p1,
		const Vertex &p2, const Vector3D &normal) const
	{
		float n_dot_dir = normal.dotProduct(ray.getDirection());
		// no intersection.
		if (equal(n_dot_dir, 0.0))
			return false;
		float d = -normal.dotProduct(p0.m_position);
		float t = -(normal.dotProduct(ray.getOrigin()) + d) / n_dot_dir;
		if (t < t_min || t > t_max)
			return false;
		ret.m_t = t;
		ret.m_position = ray.pointAt(t);
		ret.m_material = m_material;
		// judge inside or not.
		Vector3D r = ret.m_position - p0.m_position;
		Vector3D q1 = p1.m_position - p0.m_position;
		Vector3D q2 = p2.m_position - p0.m_position;
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
		ret.m_normal = p0.m_normal * (1.0f - omega1 - omega2) + p1.m_normal * omega1 + p2.m_normal * omega2;
		ret.m_texcoord = p0.m_texcoord * (1.0f - omega1 - omega2) + p1.m_texcoord * omega1 + p2.m_texcoord * omega2;
		if (ret.m_normal.dotProduct(ray.getDirection()) > 0.0f)
			ret.m_normal = -ret.m_normal;
		return true;
	}

}
