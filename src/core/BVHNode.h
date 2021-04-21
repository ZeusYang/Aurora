#ifndef BVHNODE_H
#define BVHNODE_H

#include "ArAurora.h"
#include "AABB.h"
#include "Hitable.h"

#include <iostream>

namespace Aurora
{

	class BVHNode : public Hitable
	{
	private:
		Hitable *m_left;
		Hitable *m_right;
		AABB m_box;

	public:
		BVHNode() : Hitable(nullptr) {}
		BVHNode(std::vector<Hitable*> &list, int start, int end);

		Hitable *getLeftChild() { return m_left; }
		Hitable *getRightChild() { return m_right; }

		virtual bool isLeaf() const { return false; }
		virtual bool hit(const Ray &ray, const Float &t_min, const Float &t_max, HitRecord &ret) const;
		virtual bool boundingBox(const Float &t0, const Float &t1, AABB &box) const;

		static void destoryBVHTree(Hitable *root);

		static bool boxCompareX(const Hitable *a, const Hitable *b)
		{
			AABB boxLeft, boxRight;
			if (!a->boundingBox(0, 0, boxLeft) || !b->boundingBox(0, 0, boxRight))
				std::cerr << "no bounding box in BVHNode constructor\n";
			return boxLeft.getMin().x < boxRight.getMin().x;
		}

		static bool boxCompareY(const Hitable *a, const Hitable *b)
		{
			AABB boxLeft, boxRight;
			if (!a->boundingBox(0, 0, boxLeft) || !b->boundingBox(0, 0, boxRight))
				std::cerr << "no bounding box in BVHNode constructor\n";
			return boxLeft.getMin().y < boxRight.getMin().y;
		}

		static bool boxCompareZ(const Hitable *a, const Hitable *b)
		{
			AABB boxLeft, boxRight;
			if (!a->boundingBox(0, 0, boxLeft) || !b->boundingBox(0, 0, boxRight))
				std::cerr << "no bounding box in BVHNode constructor\n";
			return boxLeft.getMin().z < boxRight.getMin().z;
		}
	};

}

#endif // BVHNODE_H
