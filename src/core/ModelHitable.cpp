#include "ModelHitable.h"

#include <fstream>
#include <sstream>

using namespace std;

namespace RayTracer
{

	ModelHitable::ModelHitable(const std::string &path, Vector3D pos, Vector3D len, unsigned int mat)
	{
		m_material = mat;
		loadObjFile(path);
		translate(pos - m_center);
		scale(len /** Vector3D(1.0f/m_scale.x, 1.0f/m_scale.y, 1.0f/m_scale.z)*/);
	}


	void ModelHitable::loadObjFile(const std::string &path)
	{
		// obj loader.
		ifstream in;
		in.open(path, ifstream::in);
		if (in.fail())
		{
			std::cout << "Fail to load obj->" << path << endl;
		}
		string line;
		vector<Vector3D> vertices;
		vector<Vector3D> normals;
		vector<Vector2D> texcoords;
		Vector3D minPoint(+FLT_MAX, +FLT_MAX, +FLT_MAX);
		Vector3D maxPoint(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		while (!in.eof())
		{
			getline(in, line);
			istringstream iss(line.c_str());
			char trash;
			//vertex
			if (!line.compare(0, 2, "v "))
			{
				iss >> trash;
				Vector3D vertex;
				iss >> vertex.x;
				iss >> vertex.y;
				iss >> vertex.z;
				vertices.push_back(vertex);

				if (minPoint.x > vertex.x)minPoint.x = vertex.x;
				if (minPoint.y > vertex.y)minPoint.y = vertex.y;
				if (minPoint.z > vertex.z)minPoint.z = vertex.z;
				if (maxPoint.x < vertex.x)maxPoint.x = vertex.x;
				if (maxPoint.y < vertex.y)maxPoint.y = vertex.y;
				if (maxPoint.z < vertex.z)maxPoint.z = vertex.z;
			}
			// normal
			else if (!line.compare(0, 3, "vn "))
			{
				iss >> trash >> trash;
				Vector3D normal;
				iss >> normal.x;
				iss >> normal.y;
				iss >> normal.z;
				normal.normalize();
				normals.push_back(normal);
			}
			// texcoord
			else if (!line.compare(0, 3, "vt "))
			{
				iss >> trash >> trash;
				Vector2D texcoord;
				iss >> texcoord.x;
				iss >> texcoord.y;
				texcoords.push_back(texcoord);
			}
			// face
			else if (!line.compare(0, 2, "f "))
			{
				iss >> trash;
				int index[3];
				while (iss >> index[0] >> trash >> index[1] >> trash >> index[2])
				{
					Vertex data;
					data.m_position = vertices[index[0] - 1];
					data.m_texcoord = texcoords[index[1] - 1];
					data.m_normal = normals[index[2] - 1];
					m_indices.push_back(m_vertices.size());
					m_vertices.push_back(data);
				}
			}
		}
		in.close();
		m_center = (maxPoint + minPoint) / 2;
		m_scale.x = maxPoint.x - minPoint.x;
		m_scale.y = maxPoint.y - minPoint.y;
		m_scale.z = maxPoint.z - minPoint.z;

	}

}
