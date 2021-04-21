#include "Skybox.h"

#include "Texture.h"
#include "TextureMgr.h"


namespace RayTracer
{

	Skybox::Skybox(const std::vector<unsigned int> &skymap)
	{
		m_cubemap[0] = skymap[0];
		m_cubemap[1] = skymap[1];
		m_cubemap[2] = skymap[2];
		m_cubemap[3] = skymap[3];
		m_cubemap[4] = skymap[4];
		m_cubemap[5] = skymap[5];

		m_vertices.resize(24);
		m_indices.resize(36);

		const float size = 1.0f;
		// front
		m_vertices[0].m_position = Vector3D(+size, +size, +size);
		m_vertices[0].m_normal = Vector3D(+0, +0, -1);
		m_vertices[0].m_texcoord = Vector2D(0.0, 1.0);
		m_vertices[1].m_position = Vector3D(-size, +size, +size);
		m_vertices[1].m_normal = Vector3D(+0, +0, -1);
		m_vertices[1].m_texcoord = Vector2D(1.0, 1.0);
		m_vertices[2].m_position = Vector3D(-size, -size, +size);
		m_vertices[2].m_normal = Vector3D(+0, +0, -1);
		m_vertices[2].m_texcoord = Vector2D(1.0, 0.0);
		m_vertices[3].m_position = Vector3D(+size, -size, +size);
		m_vertices[3].m_normal = Vector3D(+0, +0, -1);
		m_vertices[3].m_texcoord = Vector2D(0.0, 0.0);
		m_indices[0] = 0; m_indices[1] = 1; m_indices[2] = 2;
		m_indices[3] = 0; m_indices[4] = 2; m_indices[5] = 3;

		// back
		m_vertices[4].m_position = Vector3D(+size, +size, -size);
		m_vertices[4].m_normal = Vector3D(+0, +0, +1);
		m_vertices[4].m_texcoord = Vector2D(1.0, 1.0);
		m_vertices[5].m_position = Vector3D(+size, -size, -size);
		m_vertices[5].m_normal = Vector3D(+0, +0, +1);
		m_vertices[5].m_texcoord = Vector2D(1.0, 0.0);
		m_vertices[6].m_position = Vector3D(-size, -size, -size);
		m_vertices[6].m_normal = Vector3D(+0, +0, +1);
		m_vertices[6].m_texcoord = Vector2D(0.0, 0.0);
		m_vertices[7].m_position = Vector3D(-size, +size, -size);
		m_vertices[7].m_normal = Vector3D(+0, +0, +1);
		m_vertices[7].m_texcoord = Vector2D(0.0, 1.0);
		m_indices[6] = 4; m_indices[7] = 5; m_indices[8] = 6;
		m_indices[9] = 4; m_indices[10] = 6; m_indices[11] = 7;

		// left
		m_vertices[8].m_position = Vector3D(-size, +size, +size);
		m_vertices[8].m_normal = Vector3D(+1, +0, +0);
		m_vertices[8].m_texcoord = Vector2D(0.0, 1.0);
		m_vertices[9].m_position = Vector3D(-size, +size, -size);
		m_vertices[9].m_normal = Vector3D(+1, +0, +0);
		m_vertices[9].m_texcoord = Vector2D(1.0, 1.0);
		m_vertices[10].m_position = Vector3D(-size, -size, -size);
		m_vertices[10].m_normal = Vector3D(+1, +0, +0);
		m_vertices[10].m_texcoord = Vector2D(1.0, 0.0);
		m_vertices[11].m_position = Vector3D(-size, -size, +size);
		m_vertices[11].m_normal = Vector3D(+1, +0, +0);
		m_vertices[11].m_texcoord = Vector2D(0.0, 0.0);
		m_indices[12] = 8; m_indices[13] = 9; m_indices[14] = 10;
		m_indices[15] = 8; m_indices[16] = 10; m_indices[17] = 11;

		// right
		m_vertices[12].m_position = Vector3D(+size, +size, -size);
		m_vertices[12].m_normal = Vector3D(-1, +0, +0);
		m_vertices[12].m_texcoord = Vector2D(0.0, 1.0);
		m_vertices[13].m_position = Vector3D(+size, +size, +size);
		m_vertices[13].m_normal = Vector3D(-1, +0, +0);
		m_vertices[13].m_texcoord = Vector2D(1.0, 1.0);
		m_vertices[14].m_position = Vector3D(+size, -size, +size);
		m_vertices[14].m_normal = Vector3D(-1, +0, +0);
		m_vertices[14].m_texcoord = Vector2D(1.0, 0.0);
		m_vertices[15].m_position = Vector3D(+size, -size, -size);
		m_vertices[15].m_normal = Vector3D(-1, +0, +0);
		m_vertices[15].m_texcoord = Vector2D(0.0, 0.0);
		m_indices[18] = 12; m_indices[19] = 13; m_indices[20] = 14;
		m_indices[21] = 12; m_indices[22] = 14; m_indices[23] = 15;

		// top
		m_vertices[16].m_position = Vector3D(+size, +size, -size);
		m_vertices[16].m_normal = Vector3D(+0, -1, +0);
		m_vertices[16].m_texcoord = Vector2D(1.0, 0.0);
		m_vertices[17].m_position = Vector3D(-size, +size, -size);
		m_vertices[17].m_normal = Vector3D(+0, -1, +0);
		m_vertices[17].m_texcoord = Vector2D(0.0, 0.0);
		m_vertices[18].m_position = Vector3D(-size, +size, +size);
		m_vertices[18].m_normal = Vector3D(+0, -1, +0);
		m_vertices[18].m_texcoord = Vector2D(0.0, 1.0);
		m_vertices[19].m_position = Vector3D(+size, +size, +size);
		m_vertices[19].m_normal = Vector3D(+0, -1, +0);
		m_vertices[19].m_texcoord = Vector2D(1.0, 1.0);
		m_indices[24] = 16; m_indices[25] = 17; m_indices[26] = 18;
		m_indices[27] = 16; m_indices[28] = 18; m_indices[29] = 19;

		// bottom
		m_vertices[20].m_position = Vector3D(+size, -size, -size);
		m_vertices[20].m_normal = Vector3D(+0, +1, +0);
		m_vertices[20].m_texcoord = Vector2D(1.0, 1.0);
		m_vertices[21].m_position = Vector3D(+size, -size, +size);
		m_vertices[21].m_normal = Vector3D(+0, +1, +0);
		m_vertices[21].m_texcoord = Vector2D(1.0, 0.0);
		m_vertices[22].m_position = Vector3D(-size, -size, +size);
		m_vertices[22].m_normal = Vector3D(+0, +1, +0);
		m_vertices[22].m_texcoord = Vector2D(0.0, 0.0);
		m_vertices[23].m_position = Vector3D(-size, -size, -size);
		m_vertices[23].m_normal = Vector3D(+0, +1, +0);
		m_vertices[23].m_texcoord = Vector2D(0.0, 1.0);
		m_indices[30] = 20; m_indices[31] = 21; m_indices[32] = 22;
		m_indices[33] = 20; m_indices[34] = 22; m_indices[35] = 23;
	}

	Skybox::~Skybox()
	{

	}

	Vector3D Skybox::sampleBackground(const Ray &ray)
	{
		HitRecord rec;
		Ray r(Vector3D(0, 0, 0), ray.getDirection());
		TextureMgr::ptr texMgr = TextureMgr::getSingleton();
		int index = -1;
		for (int x = 0; x < m_indices.size(); x += 3)
		{
			int index1 = m_indices[x + 0];
			int index2 = m_indices[x + 1];
			int index3 = m_indices[x + 2];
			if (triangleHit(r, 0.001f, FLT_MAX, rec,
				m_vertices[index1], m_vertices[index2], m_vertices[index3],
				m_vertices[index1].m_normal))
			{
				index = x;
				break;
			}
		}

		if (index != -1)
		{
			int map = index / 6;
			return texMgr->getTexture(m_cubemap[map])
				->sample(rec.m_texcoord.x, rec.m_texcoord.y, rec.m_position);
		}
		else
			return Vector3D(0.0, 0.0, 0.0);
	}

	bool Skybox::triangleHit(const Ray &ray, const float &t_min, const float &t_max,
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
		ret.m_material = 0;
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

		//
		float omega1 = determinant * (q2_squaredLen * r_dot_q1 - q1_dot_q2 * r_dot_q2);
		float omega2 = determinant * (-q1_dot_q2 * r_dot_q1 + q1_squaredLen * r_dot_q2);
		if (omega1 + omega2 > 1.0f || omega1 < 0.0f || omega2 < 0.0f)
			return false;
		ret.m_texcoord = p0.m_texcoord * (1.0f - omega1 - omega2) + p1.m_texcoord * omega1 + p2.m_texcoord * omega2;
		return true;
	}



}
