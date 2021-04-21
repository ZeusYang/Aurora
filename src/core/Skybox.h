#ifndef SKYBOX_H
#define SKYBOX_H

#include "SimpleMeshHitable.h"

/**
 * @projectName   RayTracer
 * @brief         Sky box.
 * @author        YangWC
 * @date          2019-05-22
 */

namespace RayTracer
{

	class Skybox
	{
	private:
		// front->back->left->right->top->bottom.
		std::vector<Vertex> m_vertices;
		std::vector<unsigned int> m_indices;
		unsigned int m_cubemap[6];

	public:
		Skybox(const std::vector<unsigned int> &skymap);
		~Skybox();

		Vector3D sampleBackground(const Ray &ray);

	private:
		bool triangleHit(const Ray &ray, const float &t_min, const float &t_max,
			HitRecord &ret, const Vertex &p0, const Vertex &p1,
			const Vertex &p2, const Vector3D &normal) const;

	};

}

#endif // SKYBOX_H
