#ifndef MESHHITABLE_H
#define MESHHITABLE_H

#include "Octree.h"
#include "Hitable.h"
#include "Transform3D.h"

/**
 * @projectName   RayTracer
 * @brief         Triangle mesh.
 * @author        YangWC
 * @date          2019-05-12
 */

namespace RayTracer
{

	class MeshHitable : public Hitable
	{
	public:
		AABB m_box;
		Transform3D m_transformation;
		unsigned int m_material;
		std::vector<Vertex> m_vertices;
		std::vector<unsigned int> m_indices;
		std::vector<Vector3D> m_faceNormal;

		MeshHitable() = default;
		virtual ~MeshHitable() = default;

		void setVertices(const std::vector<Vertex> &ver, const std::vector<unsigned int> &ind)
		{
			m_vertices = ver; m_indices = ind;
		}
		std::vector<Vertex> getVertices() const { return m_vertices; }
		std::vector<unsigned int> getIndices() const { return m_indices; }

		void scale(const Vector3D &ds) { m_transformation.scale(ds); }
		void translate(const Vector3D &dt) { m_transformation.translate(dt); }
		void rotate(const Vector3D &axis, float angle) { m_transformation.rotate(axis, angle); }

		virtual void preRendering();
		virtual bool hit(const Ray &ray, const float &t_min, const float &t_max, HitRecord &ret) const;
		virtual bool boundingBox(const float &t0, const float &t1, AABB &box) const;

	private:
		bool triangleHit(const Ray &ray, const float &t_min, const float &t_max,
			HitRecord &ret, const Vertex &p0, const Vertex &p1,
			const Vertex &p2, const Vector3D &normal) const;

		Octree::ptr m_octree = nullptr;
	};

}

#endif // MESHHITABLE_H
