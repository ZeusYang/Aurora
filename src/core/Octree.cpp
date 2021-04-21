#include "Octree.h"

#include "MeshHitable.h"

namespace RayTracer
{
	Octree::Octree(Vector3D min, Vector3D max, unsigned int maxDepth)
		: m_maxDepth(maxDepth)
	{
		//! make sure the bounding box is a cube.
		//Vector3D center = (min + max) * 0.5f;
		//float maxEdge = std::max(max.x - min.x, std::max(max.y - min.y, max.z - min.z));
		//m_min = center - Vector3D(maxEdge, maxEdge, maxEdge) * 0.5f;
		//m_max = center + Vector3D(maxEdge, maxEdge, maxEdge) * 0.5f;
		m_min = min;
		m_max = max;
		m_root = nullptr;
	}

	Octree::~Octree() { destory(); }

	void Octree::destory()
	{
		recursiveDestory(m_root);
		m_root = nullptr;
	}

	void Octree::build(MeshHitable * target)
	{
		if (m_root != nullptr)
			destory();

		auto &indices = target->m_indices;
		m_root = recursiveBuild(1, m_min, m_max, target, indices);
	}

	bool Octree::visit(const Ray & ray, float t_min, float t_max, 
		std::function<bool(unsigned int i1, unsigned int i2, unsigned int i3, float &t)> func)
	{
		if (m_root == nullptr)
			return false;
		return recursiveTraveling(m_root, ray, t_min, t_max, func);
	}

	void Octree::recursiveDestory(Node * node)
	{
		//! backward traverse for deletion.
		if (node == nullptr)
			return;

		recursiveDestory(node->m_children[0]);
		recursiveDestory(node->m_children[1]);
		recursiveDestory(node->m_children[2]);
		recursiveDestory(node->m_children[3]);
		recursiveDestory(node->m_children[4]);
		recursiveDestory(node->m_children[5]);
		recursiveDestory(node->m_children[6]);
		recursiveDestory(node->m_children[7]);

		delete node;
		node = nullptr;
	}

	Node *Octree::recursiveBuild(unsigned int depth, Vector3D min, Vector3D max,
		const MeshHitable * target, std::vector<unsigned int> ids)
	{
		//! reach the max depth or there are just a few objects, we dont' need to go further.
		if (depth == m_maxDepth || ids.size() <= 10 * 3)
		{
			Node *cur = new Node(min, max, true);
			const auto &vertices = target->m_vertices;
			for (size_t i = 0; i < ids.size(); i += 3)
			{
				if (isContain(cur->m_boundingBox, vertices[ids[i + 0]].m_position,
					vertices[ids[i + 1]].m_position, vertices[ids[i + 2]].m_position))
					cur->m_objectIds.push_back(std::make_tuple(ids[i + 0], ids[i + 1], ids[i + 2]));
			}
			return cur;
		}

		//! otherwise, just divide into 8 sub-nodes.
		Node *cur = new Node(min, max, false);
		std::vector<unsigned int> subIds[8];
		const auto &vertices = target->m_vertices;
		std::vector<AABB> subRegions = cur->m_boundingBox.getEightSubAABB();
		for (size_t i = 0; i < ids.size(); i += 3)
		{
			const auto &p1 = vertices[ids[i + 0]].m_position;
			const auto &p2 = vertices[ids[i + 1]].m_position;
			const auto &p3 = vertices[ids[i + 2]].m_position;
			for (size_t j = 0; j < 8; ++j) 
			{
				if (isContain(subRegions[j], p1, p2, p3))
				{
					subIds[j].push_back(ids[i + 0]);
					subIds[j].push_back(ids[i + 1]);
					subIds[j].push_back(ids[i + 2]);
				}
			}
		}

		cur->m_children[0] = recursiveBuild(depth + 1, subRegions[0].getMin(), subRegions[0].getMax(), target, subIds[0]);
		cur->m_children[1] = recursiveBuild(depth + 1, subRegions[1].getMin(), subRegions[1].getMax(), target, subIds[1]);
		cur->m_children[2] = recursiveBuild(depth + 1, subRegions[2].getMin(), subRegions[2].getMax(), target, subIds[2]);
		cur->m_children[3] = recursiveBuild(depth + 1, subRegions[3].getMin(), subRegions[3].getMax(), target, subIds[3]);
		cur->m_children[4] = recursiveBuild(depth + 1, subRegions[4].getMin(), subRegions[4].getMax(), target, subIds[4]);
		cur->m_children[5] = recursiveBuild(depth + 1, subRegions[5].getMin(), subRegions[5].getMax(), target, subIds[5]);
		cur->m_children[6] = recursiveBuild(depth + 1, subRegions[6].getMin(), subRegions[6].getMax(), target, subIds[6]);
		cur->m_children[7] = recursiveBuild(depth + 1, subRegions[7].getMin(), subRegions[7].getMax(), target, subIds[7]);

		return cur;
	}

	bool Octree::recursiveTraveling(Node * node, const Ray & ray, float & t_min, float & t_max, 
		std::function<bool(unsigned int i1, unsigned int i2, unsigned int i3, float &t)> func)
	{
		//! just return false if node is nullptr.
		if (node == nullptr)
			return false;

		//! if the ray doesn't intersect with the aabb box, just return false.
		if (!node->m_boundingBox.hit(ray, t_min, t_max))
			return false;

		//! if it's a leaf, travels every objects of this node.
		if (node->m_isLeaf)
		{
			bool ret = false;
			for (size_t i = 0; i < node->m_objectIds.size(); ++i)
			{
				const auto &face = node->m_objectIds[i];
				ret |= func(std::get<0>(face), std::get<1>(face), std::get<2>(face), t_max);
			}
			return ret;
		}

		//! otherwise, divide into 8 sub-nodes.
		bool ret = false;
		std::vector<AABB> subRegions = node->m_boundingBox.getEightSubAABB();
		if (subRegions[0].hit(ray, t_min, t_max))
			ret |= recursiveTraveling(node->m_children[0], ray, t_min, t_max, func);
		if (subRegions[1].hit(ray, t_min, t_max))
			ret |= recursiveTraveling(node->m_children[1], ray, t_min, t_max, func);
		if (subRegions[2].hit(ray, t_min, t_max))
			ret |= recursiveTraveling(node->m_children[2], ray, t_min, t_max, func);
		if (subRegions[3].hit(ray, t_min, t_max))
			ret |= recursiveTraveling(node->m_children[3], ray, t_min, t_max, func);
		if (subRegions[4].hit(ray, t_min, t_max))
			ret |= recursiveTraveling(node->m_children[4], ray, t_min, t_max, func);
		if (subRegions[5].hit(ray, t_min, t_max))
			ret |= recursiveTraveling(node->m_children[5], ray, t_min, t_max, func);
		if (subRegions[6].hit(ray, t_min, t_max))
			ret |= recursiveTraveling(node->m_children[6], ray, t_min, t_max, func);
		if (subRegions[7].hit(ray, t_min, t_max))
			ret |= recursiveTraveling(node->m_children[7], ray, t_min, t_max, func);

		return ret;
	}

	bool Octree::isContain(const AABB & box, const Vector3D & p1, const Vector3D & p2, const Vector3D & p3)
	{
		//! at least one point of the triangle is inside the box, return true.
		if (box.isInside(p1) || box.isInside(p2) || box.isInside(p3))
			return true;

		//! otherwise testing for each edge.
		Ray edge1(p1, p2 - p1);
		Ray edge2(p2, p3 - p2);
		Ray edge3(p3, p1 - p3);
		float length1 = (p2 - p1).getLength();
		float length2 = (p3 - p2).getLength();
		float length3 = (p1 - p3).getLength();
		if (box.hit(edge1, 0.0f, length1) || box.hit(edge2, 0.0f, length2) || box.hit(edge3, 0.0f, length3))
			return true;
		
		//! if there is no intersection at all, just return false.
		return false;
	}
}