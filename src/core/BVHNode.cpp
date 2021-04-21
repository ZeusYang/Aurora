#include "BVHNode.h"

#include <algorithm>
#include <iostream>

using namespace std;

namespace Aurora
{

	BVHNode::BVHNode(std::vector<Hitable *> &list, int start, int end)
		: Hitable(nullptr)
	{
		// sort it randomly depend on
		int axis = static_cast<int>(3 * drand48());
		if (axis == 0)
			sort(&list[start], &list[end], boxCompareX);
		else if (axis == 1)
			sort(&list[start], &list[end], boxCompareY);
		else if (axis == 2)
			sort(&list[start], &list[end], boxCompareZ);
		int length = end - start;
		if (length == 1)
			m_left = m_right = list[start];
		else if (length == 2)
		{
			m_left = list[start];
			m_right = list[start + 1];
		}
		else
		{
			m_left = new BVHNode(list, start, start + length / 2);
			m_right = new BVHNode(list, start + length / 2, end);
		}
		// bounding box.
		AABB boxLeft, boxRight;
		if (!m_left->boundingBox(0, 0, boxLeft) || !m_right->boundingBox(0, 0, boxRight))
			std::cerr << "no bounding box in BVHNode constructor\n";
		m_box = AABB::surroundingBox(boxLeft, boxRight);
	}

	bool BVHNode::hit(const Ray &ray, const Float &t_min, const Float &t_max, HitRecord &ret) const
	{
		if (m_box.hit(ray, t_min, t_max))
		{
			HitRecord leftRec, rightRec;
			bool hitLeft = m_left->hit(ray, t_min, t_max, leftRec);
			bool hitRight = m_right->hit(ray, t_min, t_max, rightRec);
			// both hit.
			if (hitLeft && hitRight)
			{
				if (leftRec.m_t < rightRec.m_t)
					ret = leftRec;
				else
					ret = rightRec;
				return true;
			}
			// only left child.
			else if (hitLeft)
			{
				ret = leftRec;
				return true;
			}
			else if (hitRight)
			{
				ret = rightRec;
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}

	bool BVHNode::boundingBox(const Float &t0, const Float &t1, AABB &box) const
	{
		box = m_box;
		return true;
	}

	void BVHNode::destoryBVHTree(Hitable *root)
	{
		if (root == nullptr || root->isLeaf())
			return;
		BVHNode *broot = reinterpret_cast<BVHNode*>(root);
		destoryBVHTree(broot->m_left);
		destoryBVHTree(broot->m_right);
		delete broot;
		root = nullptr;
	}

}
