#include "SimpleMeshHitable.h"

using namespace std;

namespace RayTracer
{

	Triangle::Triangle(Vector3D pos, Vector3D len, unsigned int mat)
	{
		translate(pos);
		scale(len);
		std::vector<Vertex> vertices(3);
		std::vector<unsigned int> indices(3);
		vertices[0].m_position = Vector3D(0, 1, 0);
		vertices[0].m_normal = Vector3D(0, 0, 1);
		vertices[0].m_texcoord = Vector2D(0.5, 0.5);

		vertices[1].m_position = Vector3D(-1, 0, 0);
		vertices[1].m_normal = Vector3D(0, 0, 1);
		vertices[1].m_texcoord = Vector2D(0.0, 0.0);

		vertices[2].m_position = Vector3D(+1, 0, 0);
		vertices[2].m_normal = Vector3D(0, 0, 1);
		vertices[2].m_texcoord = Vector2D(1.0, 0.0);

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		setVertices(vertices, indices);
		m_material = mat;
	}

	Cube::Cube(Vector3D pos, Vector3D len, unsigned int mat)
	{
		translate(pos);
		scale(len);
		std::vector<Vertex> vertices(24);
		std::vector<unsigned int> indices(36);

		// front
		vertices[0].m_position = Vector3D(+1, +1, +1);
		vertices[0].m_normal = Vector3D(+0, +0, +1);
		vertices[0].m_texcoord = Vector2D(1.0, 1.0);
		vertices[1].m_position = Vector3D(-1, +1, +1);
		vertices[1].m_normal = Vector3D(+0, +0, +1);
		vertices[1].m_texcoord = Vector2D(0.0, 1.0);
		vertices[2].m_position = Vector3D(-1, -1, +1);
		vertices[2].m_normal = Vector3D(+0, +0, +1);
		vertices[2].m_texcoord = Vector2D(0.0, 0.0);
		vertices[3].m_position = Vector3D(+1, -1, +1);
		vertices[3].m_normal = Vector3D(+0, +0, +1);
		vertices[3].m_texcoord = Vector2D(1.0, 0.0);
		indices[0] = 0; indices[1] = 1; indices[2] = 2;
		indices[3] = 0; indices[4] = 2; indices[5] = 3;

		// back
		vertices[4].m_position = Vector3D(+1, +1, -1);
		vertices[4].m_normal = Vector3D(+0, +0, -1);
		vertices[4].m_texcoord = Vector2D(1.0, 1.0);
		vertices[5].m_position = Vector3D(+1, -1, -1);
		vertices[5].m_normal = Vector3D(+0, +0, -1);
		vertices[5].m_texcoord = Vector2D(0.0, 1.0);
		vertices[6].m_position = Vector3D(-1, -1, -1);
		vertices[6].m_normal = Vector3D(+0, +0, -1);
		vertices[6].m_texcoord = Vector2D(0.0, 0.0);
		vertices[7].m_position = Vector3D(-1, +1, -1);
		vertices[7].m_normal = Vector3D(+0, +0, -1);
		vertices[7].m_texcoord = Vector2D(1.0, 0.0);
		indices[6] = 4; indices[7] = 5; indices[8] = 6;
		indices[9] = 4; indices[10] = 6; indices[11] = 7;

		// left
		vertices[8].m_position = Vector3D(-1, +1, +1);
		vertices[8].m_normal = Vector3D(-1, +0, +0);
		vertices[8].m_texcoord = Vector2D(1.0, 1.0);
		vertices[9].m_position = Vector3D(-1, +1, -1);
		vertices[9].m_normal = Vector3D(-1, +0, +0);
		vertices[9].m_texcoord = Vector2D(0.0, 1.0);
		vertices[10].m_position = Vector3D(-1, -1, -1);
		vertices[10].m_normal = Vector3D(-1, +0, +0);
		vertices[10].m_texcoord = Vector2D(0.0, 0.0);
		vertices[11].m_position = Vector3D(-1, -1, +1);
		vertices[11].m_normal = Vector3D(-1, +0, +0);
		vertices[11].m_texcoord = Vector2D(1.0, 0.0);
		indices[12] = 8; indices[13] = 9; indices[14] = 10;
		indices[15] = 8; indices[16] = 10; indices[17] = 11;

		// right
		vertices[12].m_position = Vector3D(+1, +1, -1);
		vertices[12].m_normal = Vector3D(+1, +0, +0);
		vertices[12].m_texcoord = Vector2D(1.0, 1.0);
		vertices[13].m_position = Vector3D(+1, +1, +1);
		vertices[13].m_normal = Vector3D(+1, +0, +0);
		vertices[13].m_texcoord = Vector2D(0.0, 1.0);
		vertices[14].m_position = Vector3D(+1, -1, +1);
		vertices[14].m_normal = Vector3D(+1, +0, +0);
		vertices[14].m_texcoord = Vector2D(0.0, 0.0);
		vertices[15].m_position = Vector3D(+1, -1, -1);
		vertices[15].m_normal = Vector3D(+1, +0, +0);
		vertices[15].m_texcoord = Vector2D(1.0, 0.0);
		indices[18] = 12; indices[19] = 13; indices[20] = 14;
		indices[21] = 12; indices[22] = 14; indices[23] = 15;

		// top
		vertices[16].m_position = Vector3D(+1, +1, -1);
		vertices[16].m_normal = Vector3D(+0, +1, +0);
		vertices[16].m_texcoord = Vector2D(1.0, 1.0);
		vertices[17].m_position = Vector3D(-1, +1, -1);
		vertices[17].m_normal = Vector3D(+0, +1, +0);
		vertices[17].m_texcoord = Vector2D(0.0, 1.0);
		vertices[18].m_position = Vector3D(-1, +1, +1);
		vertices[18].m_normal = Vector3D(+0, +1, +0);
		vertices[18].m_texcoord = Vector2D(0.0, 0.0);
		vertices[19].m_position = Vector3D(+1, +1, +1);
		vertices[19].m_normal = Vector3D(+0, +1, +0);
		vertices[19].m_texcoord = Vector2D(1.0, 0.0);
		indices[24] = 16; indices[25] = 17; indices[26] = 18;
		indices[27] = 16; indices[28] = 18; indices[29] = 19;

		// bottom
		vertices[20].m_position = Vector3D(+1, -1, -1);
		vertices[20].m_normal = Vector3D(+0, -1, +0);
		vertices[20].m_texcoord = Vector2D(1.0, 1.0);
		vertices[21].m_position = Vector3D(+1, -1, +1);
		vertices[21].m_normal = Vector3D(+0, -1, +0);
		vertices[21].m_texcoord = Vector2D(0.0, 1.0);
		vertices[22].m_position = Vector3D(-1, -1, +1);
		vertices[22].m_normal = Vector3D(+0, -1, +0);
		vertices[22].m_texcoord = Vector2D(0.0, 0.0);
		vertices[23].m_position = Vector3D(-1, -1, -1);
		vertices[23].m_normal = Vector3D(+0, -1, +0);
		vertices[23].m_texcoord = Vector2D(1.0, 0.0);
		indices[30] = 20; indices[31] = 21; indices[32] = 22;
		indices[33] = 20; indices[34] = 22; indices[35] = 23;

		setVertices(vertices, indices);
		m_material = mat;
	}

	Plane::Plane(Vector3D pos, Vector3D len, unsigned int mat)
	{
		translate(pos);
		scale(len);
		std::vector<Vertex> vertices(4);
		std::vector<unsigned int> indices(6);
		vertices[0].m_position = Vector3D(-1, 0, -1);
		vertices[0].m_normal = Vector3D(0, 1, 0);
		vertices[0].m_texcoord = Vector2D(0.0, 1.0);

		vertices[1].m_position = Vector3D(-1, 0, +1);
		vertices[1].m_normal = Vector3D(0, 1, 0);
		vertices[1].m_texcoord = Vector2D(0.0, 0.0);

		vertices[2].m_position = Vector3D(+1, 0, -1);
		vertices[2].m_normal = Vector3D(0, 1, 0);
		vertices[2].m_texcoord = Vector2D(1.0, 1.0);

		vertices[3].m_position = Vector3D(+1, 0, +1);
		vertices[3].m_normal = Vector3D(0, 1, 0);
		vertices[3].m_texcoord = Vector2D(1.0, 0.0);

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		indices[3] = 2;
		indices[4] = 1;
		indices[5] = 3;

		setVertices(vertices, indices);

		m_material = mat;
	}

	Vector3D Plane::random(const Vector3D &o) const
	{
		Vector3D center = m_transformation.translation();
		Vector3D leftCorner;
		float width = m_transformation.scale().x * 2.0f;
		float height = m_transformation.scale().z * 2.0f;
		leftCorner.x = center.x - m_transformation.scale().x;
		leftCorner.z = center.z - m_transformation.scale().z;
		leftCorner.y = center.y;
		Vector3D random_point(leftCorner.x + drand48() * width, leftCorner.y,
			leftCorner.z + drand48() * height);
		return random_point - o;
	}

	float Plane::pdfValue(const Vector3D &o, const Vector3D &v) const
	{

		HitRecord rec;
		if (this->hit(Ray(o, v), 0.001f, FLT_MAX, rec))
		{
			float area = m_transformation.scale().x * 2.0f * m_transformation.scale().z * 2.0f;
			float distance_squared = v.getSquaredLength();
			float cosine = fabs(v.dotProduct(rec.m_normal) / v.getLength());
			float ret = distance_squared / (cosine * area);
			return ret;
		}
		else
			return 0.0f;
	}

}
