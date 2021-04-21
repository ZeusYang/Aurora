#pragma once

#include <tuple>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>

#include "AABB.h"
#include "Vector3D.h"

/**
 * @projectName   RayTracer
 * @brief         Octree for accelerating intersection detection of triangle mesh.
 * @author        YangWC
 * @date          2020-1-7
 */
namespace RayTracer
{
	struct Node 
	{
		//! it's leaf or not.
		bool m_isLeaf;
		//! the eight children.
		Node *m_children[8];
		//! extent.
		AABB m_boundingBox;
		//! objects' id list.
		std::vector<std::tuple<unsigned int, unsigned int, unsigned int>> m_objectIds;

		Node() : m_isLeaf(true), m_boundingBox(AABB(Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 0.0f)))
		{
			m_children[0] = m_children[1] = m_children[2] = m_children[3] = nullptr;
			m_children[4] = m_children[5] = m_children[6] = m_children[7] = nullptr;
		}

		Node(Vector3D min, Vector3D max, bool isLeaf) : m_isLeaf(isLeaf),
			m_boundingBox(AABB(min, max))
		{
			m_children[0] = m_children[1] = m_children[2] = m_children[3] = nullptr;
			m_children[4] = m_children[5] = m_children[6] = m_children[7] = nullptr;
		}

	};

	class MeshHitable;
	class Octree
	{
	public:
		typedef std::shared_ptr<Octree> ptr;

		Octree(Vector3D min, Vector3D max, unsigned int maxDepth = 8);
		~Octree();

		//! destory the tree.
		void destory();

		//! build the tree from meshes.
		void build(MeshHitable *target);

		//£¡ visit.
		bool visit(const Ray &ray, float t_min, float t_max,
			std::function<bool(unsigned int i1, unsigned int i2, unsigned int i3, float &t)> func);

	private:
		Node *m_root;
		Vector3D m_min, m_max;
		unsigned int m_maxDepth;

		//! destory the whole tree recursively.
		void recursiveDestory(Node *node);

		//! build the whole tree recursively.
		Node *recursiveBuild(unsigned int depth, Vector3D min, Vector3D max,
			const MeshHitable * target, std::vector<unsigned int> ids);

		//! traveling the octree.
		bool recursiveTraveling(Node *node, const Ray &ray, float &t_min, float &t_max,
			std::function<bool(unsigned int i1, unsigned int i2, unsigned int i3, float &t)> func);

		//! judge whether the triangle intersects with the bounding box.
		bool isContain(const AABB &box, const Vector3D &p1, const Vector3D &p2, const Vector3D &p3);
			
	};
}