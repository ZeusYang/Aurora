#include "MeshHitable.h"

namespace Aurora
{

	void MeshHitable::preRendering()
	{
		// transform and calculate aabb box.
		if (!m_transformation.getDirtry() || m_indices.empty())
			return;

		AVector3f minPoint(+FLT_MAX, +FLT_MAX, +FLT_MAX);
		AVector3f maxPoint(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		AMatrix4x4 modelMatrix = m_transformation.toMatrix();
		AMatrix4x4 invModelMatrix = m_transformation.toInvMatrix();
		AVector3f pos, nor;
		unsigned int p0, p1, p2;
		for (int x = 0; x < m_vertices.size(); ++x)
		{
			int index = x;
			pos = m_vertices[index].m_position;
			nor = m_vertices[index].m_normal;
			pos = modelMatrix * glm::vec4(pos, 1.0f);
			nor = invModelMatrix * glm::vec4(nor, 0.0f);
			m_vertices[index].m_position = pos;
			m_vertices[index].m_normal = normalize(nor);
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
			AVector3f normal = normalize(cross((m_vertices[index1].m_position - m_vertices[index0].m_position)
				,m_vertices[index2].m_position - m_vertices[index0].m_position));
			m_faceNormal.push_back(normal);
		}
	}

	bool MeshHitable::hit(const ARay &ray, const Float &t_min, const Float &t_max, HitRecord &ret) const
	{
		HitRecord tmpRec;
		bool hitAny = false;
		
		//! brute-force algorithm.
		Float closestSoFar = t_max;
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

		return hitAny;
	}

	bool MeshHitable::triangleHit(const ARay &ray, const Float &t_min, const Float &t_max,
		HitRecord &ret, const Vertex &p0, const Vertex &p1,
		const Vertex &p2, const AVector3f &normal) const
	{
		Float n_dot_dir = dot(normal, ray.direction());
		// no intersection.
		if (equal(n_dot_dir, 0.0))
			return false;
		Float d = -dot(normal, p0.m_position);
		Float t = -(dot(normal, ray.origin()) + d) / n_dot_dir;
		if (t < t_min || t > t_max)
			return false;
		ret.m_t = t;
		ret.m_position = ray(t);
		ret.m_material = m_material.get();
		// judge inside or not.
		AVector3f r = ret.m_position - p0.m_position;
		AVector3f q1 = p1.m_position - p0.m_position;
		AVector3f q2 = p2.m_position - p0.m_position;
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
		ret.m_normal = p0.m_normal * (1.0f - omega1 - omega2) + p1.m_normal * omega1 + p2.m_normal * omega2;
		ret.m_texcoord = p0.m_texcoord * (1.0f - omega1 - omega2) + p1.m_texcoord * omega1 + p2.m_texcoord * omega2;
		if (dot(ret.m_normal, ray.direction()) > 0.0f)
			ret.m_normal = -ret.m_normal;
		return true;
	}

	Plane::Plane(const Material::ptr &mat, AVector3f pos, AVector3f len)
		: MeshHitable(mat)
	{
		translate(pos);
		scale(len);
		std::vector<Vertex> vertices(4);
		std::vector<unsigned int> indices(6);
		vertices[0].m_position = AVector3f(-1, 0, -1);
		vertices[0].m_normal = AVector3f(0, 1, 0);
		vertices[0].m_texcoord = AVector2f(0.0, 1.0);

		vertices[1].m_position = AVector3f(-1, 0, +1);
		vertices[1].m_normal = AVector3f(0, 1, 0);
		vertices[1].m_texcoord = AVector2f(0.0, 0.0);

		vertices[2].m_position = AVector3f(+1, 0, -1);
		vertices[2].m_normal = AVector3f(0, 1, 0);
		vertices[2].m_texcoord = AVector2f(1.0, 1.0);

		vertices[3].m_position = AVector3f(+1, 0, +1);
		vertices[3].m_normal = AVector3f(0, 1, 0);
		vertices[3].m_texcoord = AVector2f(1.0, 0.0);

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		indices[3] = 2;
		indices[4] = 1;
		indices[5] = 3;

		setVertices(vertices, indices);

		m_material = mat;
	}

	AVector3f Plane::random(const AVector3f &o) const
	{
		AVector3f center = m_transformation.translation();
		AVector3f leftCorner;
		Float width = m_transformation.scale().x * 2.0f;
		Float height = m_transformation.scale().z * 2.0f;
		leftCorner.x = center.x - m_transformation.scale().x;
		leftCorner.z = center.z - m_transformation.scale().z;
		leftCorner.y = center.y;
		AVector3f random_point(leftCorner.x + drand48() * width, leftCorner.y,
			leftCorner.z + drand48() * height);
		return random_point - o;
	}

	Float Plane::pdfValue(const AVector3f &o, const AVector3f &v) const
	{

		HitRecord rec;
		if (this->hit(ARay(o, v), 0.001f, FLT_MAX, rec))
		{
			Float area = m_transformation.scale().x * 2.0f * m_transformation.scale().z * 2.0f;
			Float distance_squared = lengthSquared(v);
			Float cosine = fabs(dot(v, rec.m_normal) / length(v));
			Float ret = distance_squared / (cosine * area);
			return ret;
		}
		else
			return 0.0f;
	}

}
