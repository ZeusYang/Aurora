#ifndef ARKDTREE_H
#define ARKDTREE_H

#include "ArAurora.h"
#include "ArMathUtils.h"
#include "ArHitable.h"

namespace Aurora
{
	class AKdTreeNode;
	class ABoundEdge;
	class AKdTree : public AHitableAggregate
	{
	public:
		typedef std::shared_ptr<AKdTree> ptr;

		AKdTree(const std::vector<AHitable::ptr> &hitables, int isectCost = 80, int traversalCost = 1,
			Float emptyBonus = 0.5, int maxPrims = 1, int maxDepth = -1);

		virtual ABounds3f worldBound() const override { return m_bounds; }
		~AKdTree();

		virtual bool hit(const ARay &ray) const override;
		virtual bool hit(const ARay &ray, ASurfaceInteraction &iset) const override;

		virtual std::string toString() const override { return "KdTree[]"; }

	private:

		void buildTree(int nodeNum, const ABounds3f &bounds,
			const std::vector<ABounds3f> &primBounds, int *primNums,
			int nprims, int depth,
			const std::unique_ptr<ABoundEdge[]> edges[3], int *prims0,
			int *prims1, int badRefines = 0);
		
		// SAH split measurement
		const Float m_emptyBonus;
		const int m_isectCost, m_traversalCost, m_maxHitables;

		// Compact the node into an array
		AKdTreeNode *m_nodes;
		int m_nAllocedNodes, m_nextFreeNode;
		
		ABounds3f m_bounds;
		std::vector<AHitable::ptr> m_hitables;
		std::vector<int> m_hitableIndices;
	};

	struct AKdToDo 
	{
		const AKdTreeNode *node;
		Float tMin, tMax;
	};
}

#endif