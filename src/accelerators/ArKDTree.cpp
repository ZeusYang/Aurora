#include "ArKDTree.h"

#include "ArMemory.h"

namespace Aurora
{
	class AKdTreeNode 
	{
	public:

		void initLeafNode(int *hitableIndices, int np, std::vector<int> *primitiveIndices);

		void initInteriorNode(int axis, int ac, Float split) 
		{
			// Note: we lay the nodes out in a way that lets us only store one child pointer : 
			//		 all of the nodes are allocated in a single contiguous block of
			//		 memory, and the child of an interior node that is responsible for space below the splitting
			//		 plane is always stored in the array position immediately after its parent
			m_split = split;
			m_flags = axis;

			// Note: The other child, representing space above the splitting plane, will end up somewhere else
			//       in the array, and m_aboveChild stores its position in the nodes array
			m_rightChildIndex |= (ac << 2);
		}

		Float splitPos() const { return m_split; }
		int numHitables() const { return m_nHitables >> 2; }
		int splitAxis() const { return m_flags & 3; }
		bool isLeaf() const { return (m_flags & 3) == 3; }
		int aboveChild() const { return m_rightChildIndex >> 2; }

		union 
		{
			Float m_split;                 // Split position which is for interior nodes
			int m_oneHitable;              // Leaf
			int m_hitableIndicesOffset;  // Leaf
		};

	private:
		union 
		{
			int m_flags;		  // Both
			int m_nHitables;	  // Leaf
			int m_rightChildIndex;// Interior
		};
	};

	void AKdTreeNode::initLeafNode(int *hitableIndices, int np, std::vector<int> *primitiveIndices)
	{
		// Note: the low 2 bits of m_flags which holds the value 3 indicate that it's a leaf node
		//       the upper 30 bits of m_nPrims are available to record how many hitables overlap it.
		m_flags = 3;
		m_nHitables |= (np << 2);

		// Store hitable ids for leaf node
		if (np == 0)
		{
			m_oneHitable = 0;
		}
		else if (np == 1)
		{
			m_oneHitable = hitableIndices[0];
		}
		else 
		{
			// Note: if more than one primitive overlaps, then their indices are
			//       stored in a segment of m_primitiveIndicesOffset. The offset
			//       to the first index for the leaf is stored in m_primitiveIndicesOffset
			m_hitableIndicesOffset = primitiveIndices->size();
			for (int i = 0; i < np; ++i)
			{
				primitiveIndices->push_back(hitableIndices[i]);
			}
		}
	}

	enum class AEdgeType { Start, End };
	class ABoundEdge 
	{
	public:
		ABoundEdge() = default;
		ABoundEdge(Float t, int hitableIndex, bool starting) : m_t(t), m_hitableIndex(hitableIndex)
		{
			m_type = starting ? AEdgeType::Start : AEdgeType::End;
		}

		Float m_t;
		int m_hitableIndex;
		AEdgeType m_type;
	};

	AKdTree::AKdTree(const std::vector<AHitable::ptr> &hitables, int isectCost/* = 80*/, int traversalCost/* = 1*/,
		Float emptyBonus/* = 0.5*/, int maxHitables/* = 1*/, int maxDepth/* = -1*/) : 
		m_isectCost(isectCost),
		m_traversalCost(traversalCost),
		m_maxHitables(maxHitables),
		m_emptyBonus(emptyBonus),
		m_hitables(hitables)
	{
		m_nextFreeNode = m_nAllocedNodes = 0;
		if (maxDepth <= 0)
		{
			maxDepth = std::round(8 + 1.3f * glm::log2(float(int64_t(m_hitables.size()))));
		}

		// Compute bounds for kd-tree construction
		std::vector<ABounds3f> hitableBounds;
		hitableBounds.reserve(m_hitables.size());
		for (const AHitable::ptr &hitable : m_hitables) 
		{
			ABounds3f b = hitable->worldBound();
			m_bounds = unionBounds(m_bounds, b);
			hitableBounds.push_back(b);
		}

		// Allocate working memory for kd-tree construction
		std::unique_ptr<ABoundEdge[]> edges[3];
		for (int i = 0; i < 3; ++i)
		{
			edges[i].reset(new ABoundEdge[2 * m_hitables.size()]);
		}
		std::unique_ptr<int[]> leftNodeRoom(new int[m_hitables.size()]);
		std::unique_ptr<int[]> rightNodeRoom(new int[(maxDepth + 1) * m_hitables.size()]);

		// Initialize _primNums_ for kd-tree construction
		std::unique_ptr<int[]> hitableIndices(new int[m_hitables.size()]);
		for (size_t i = 0; i < m_hitables.size(); ++i)
		{
			hitableIndices[i] = i;
		}

		// Start recursive construction of kd-tree
		buildTree(0, m_bounds, hitableBounds, hitableIndices.get(), m_hitables.size(),
			maxDepth, edges, leftNodeRoom.get(), rightNodeRoom.get());
	}

	void AKdTree::buildTree(int nodeIndex, const ABounds3f &nodeBounds,
		const std::vector<ABounds3f> &allHitableBounds,
		int *hitableIndices, int nHitables, int depth,
		const std::unique_ptr<ABoundEdge[]> edges[3],
		int *leftNodeRoom, int *rightNodeRoom, int badRefines)
	{
		CHECK_EQ(nodeIndex, m_nextFreeNode);

		// Get next free node from _nodes_ array
		if (m_nextFreeNode == m_nAllocedNodes)
		{
			// Note: If all of the allocated nodes have been used up, node memory
			//		 is reallocated with twice as many entries and the old values are copied.
			int nNewAllocNodes = glm::max(2 * m_nAllocedNodes, 512);
			AKdTreeNode *n = AllocAligned<AKdTreeNode>(nNewAllocNodes);
			if (m_nAllocedNodes > 0) 
			{
				memcpy(n, m_nodes, m_nAllocedNodes * sizeof(AKdTreeNode));
				FreeAligned(m_nodes);
			}
			m_nodes = n;
			m_nAllocedNodes = nNewAllocNodes;
		}
		++m_nextFreeNode;

		// Initialize leaf node if termination criteria met
		if (nHitables <= m_maxHitables || depth == 0) 
		{
			m_nodes[nodeIndex].initLeafNode(hitableIndices, nHitables, &m_hitableIndices);
			return;
		}

		// Initialize interior node and continue recursion
		
		// Choose split axis position for interior node
		int bestAxis = -1, bestOffset = -1;
		Float bestCost = aInfinity;
		Float oldCost = m_isectCost * Float(nHitables);

		// Current node surface area
		const Float totalSA = nodeBounds.surfaceArea();
		const Float invTotalSA = 1 / totalSA;
		AVector3f diagonal = nodeBounds.m_pMax - nodeBounds.m_pMin;

		// Choose which axis to split along
		int axis = nodeBounds.maximumExtent();
		int retries = 0;

	retrySplit:
		// Initialize edges for _axis_
		for (int i = 0; i < nHitables; ++i)
		{
			int hi = hitableIndices[i];
			const ABounds3f &bounds = allHitableBounds[hi];
			edges[axis][2 * i    ] = ABoundEdge(bounds.m_pMin[axis], hi, true);
			edges[axis][2 * i + 1] = ABoundEdge(bounds.m_pMax[axis], hi, false);
		}

		// Sort _edges_ for _axis_
		std::sort(&edges[axis][0], &edges[axis][2 * nHitables],
			[](const ABoundEdge &e0, const ABoundEdge &e1) -> bool
		{
			if (e0.m_t == e1.m_t)
			{
				return (int)e0.m_type < (int)e1.m_type;
			}
			else
			{
				return e0.m_t < e1.m_t;
			}
		});

		// Compute cost of all splits for _axis_ to find best
		int nBelow = 0, nAbove = nHitables;
		for (int i = 0; i < 2 * nHitables; ++i)
		{
			if (edges[axis][i].m_type == AEdgeType::End)
				--nAbove;
			Float edgeT = edges[axis][i].m_t;
			if (edgeT > nodeBounds.m_pMin[axis] && edgeT < nodeBounds.m_pMax[axis])
			{
				// Compute cost for split at _i_th edge

				// Compute child surface areas for split at _edgeT_
				int otherAxis0 = (axis + 1) % 3, otherAxis1 = (axis + 2) % 3;
				Float belowSA = 2 * (diagonal[otherAxis0] * diagonal[otherAxis1] + (edgeT - nodeBounds.m_pMin[axis]) *
					(diagonal[otherAxis0] + diagonal[otherAxis1]));
				Float aboveSA = 2 * (diagonal[otherAxis0] * diagonal[otherAxis1] + (nodeBounds.m_pMax[axis] - edgeT) *
					(diagonal[otherAxis0] + diagonal[otherAxis1]));
				Float pBelow = belowSA * invTotalSA;
				Float pAbove = aboveSA * invTotalSA;
				Float eb = (nAbove == 0 || nBelow == 0) ? m_emptyBonus : 0;
				Float cost = m_traversalCost + m_isectCost * (1 - eb) * (pBelow * nBelow + pAbove * nAbove);

				// Update best split if this is lowest cost so far
				if (cost < bestCost)
				{
					bestCost = cost;
					bestAxis = axis;
					bestOffset = i;
				}
			}
			if (edges[axis][i].m_type == AEdgeType::Start)
				++nBelow;
		}
		CHECK(nBelow == nHitables && nAbove == 0);

		if (bestAxis == -1 && retries < 2) 
		{
			++retries;
			axis = (axis + 1) % 3;
			goto retrySplit;
		}

		// Create leaf if no good splits were found
		if (bestCost > oldCost) 
			++badRefines;
		if ((bestCost > 4 * oldCost && nHitables < 16) || bestAxis == -1 || badRefines == 3) 
		{
			m_nodes[nodeIndex].initLeafNode(hitableIndices, nHitables, &m_hitableIndices);
			return;
		}

		// Classify primitives with respect to split
		int lnHitables = 0, rnHitables = 0;
		for (int i = 0; i < bestOffset; ++i)
		{
			if (edges[bestAxis][i].m_type == AEdgeType::Start)
				leftNodeRoom[lnHitables++] = edges[bestAxis][i].m_hitableIndex;
		}
		for (int i = bestOffset + 1; i < 2 * nHitables; ++i)
		{
			if (edges[bestAxis][i].m_type == AEdgeType::End)
				rightNodeRoom[rnHitables++] = edges[bestAxis][i].m_hitableIndex;
		}

		// Recursively initialize children nodes
		Float tSplit = edges[bestAxis][bestOffset].m_t;
		ABounds3f bounds0 = nodeBounds, bounds1 = nodeBounds;
		bounds0.m_pMax[bestAxis] = bounds1.m_pMin[bestAxis] = tSplit;

		// Left subtree node
		buildTree(nodeIndex + 1, bounds0, allHitableBounds, leftNodeRoom, lnHitables, depth - 1, edges,
			leftNodeRoom, rightNodeRoom + nHitables, badRefines);
		int aboveChildIndex = m_nextFreeNode;
		m_nodes[nodeIndex].initInteriorNode(bestAxis, aboveChildIndex, tSplit);

		// Right subtree node
		buildTree(aboveChildIndex, bounds1, allHitableBounds, rightNodeRoom, rnHitables, depth - 1, edges,
			leftNodeRoom, rightNodeRoom + nHitables, badRefines);
	}

	AKdTree::~AKdTree() { FreeAligned(m_nodes); }

	bool AKdTree::hit(const ARay &ray) const
	{
		// Compute initial parametric range of ray inside kd-tree extent
		Float tMin, tMax;
		if (!m_bounds.hit(ray, tMin, tMax)) 
		{
			return false;
		}

		// Prepare to traverse kd-tree for ray
		AVector3f invDir(1 / ray.m_dir.x, 1 / ray.m_dir.y, 1 / ray.m_dir.z);
		constexpr int maxTodo = 64;
		AKdToDo todo[maxTodo];
		int todoPos = 0;
		const AKdTreeNode *currNode = &m_nodes[0];
		while (currNode != nullptr)
		{
			if (currNode->isLeaf()) 
			{
				// Check for shadow ray intersections inside leaf node
				int nHitables = currNode->numHitables();
				if (nHitables == 1)
				{
					const AHitable::ptr &p = m_hitables[currNode->m_oneHitable];
					if (p->hit(ray)) 
					{
						return true;
					}
				}
				else 
				{
					for (int i = 0; i < nHitables; ++i)
					{
						int hitableIndex = m_hitableIndices[currNode->m_hitableIndicesOffset + i];
						const AHitable::ptr &prim = m_hitables[hitableIndex];
						if (prim->hit(ray)) 
						{
							return true;
						}
					}
				}

				// Grab next node to process from todo list
				if (todoPos > 0) 
				{
					--todoPos;
					currNode = todo[todoPos].node;
					tMin = todo[todoPos].tMin;
					tMax = todo[todoPos].tMax;
				}
				else
				{
					break;
				}
			}
			else 
			{
				// Process kd-tree interior node

				// Compute parametric distance along ray to split plane
				int axis = currNode->splitAxis();
				Float tPlane = (currNode->splitPos() - ray.m_origin[axis]) * invDir[axis];

				// Get node children pointers for ray
				const AKdTreeNode *firstChild, *secondChild;
				int belowFirst = (ray.m_origin[axis] < currNode->splitPos()) ||
					(ray.m_origin[axis] == currNode->splitPos() && ray.m_dir[axis] <= 0);
				if (belowFirst) 
				{
					firstChild = currNode + 1;
					secondChild = &currNode[currNode->aboveChild()];
				}
				else 
				{
					firstChild = &currNode[currNode->aboveChild()];
					secondChild = currNode + 1;
				}

				// Advance to next child node, possibly enqueue other child
				if (tPlane > tMax || tPlane <= 0)
				{
					currNode = firstChild;
				}
				else if (tPlane < tMin)
				{
					currNode = secondChild;
				}
				else 
				{
					// Enqueue _secondChild_ in todo list
					todo[todoPos].node = secondChild;
					todo[todoPos].tMin = tPlane;
					todo[todoPos].tMax = tMax;
					++todoPos;
					currNode = firstChild;
					tMax = tPlane;
				}
			}
		}
		return false;
	}

	bool AKdTree::hit(const ARay &ray, ASurfaceInteraction &isect) const
	{
		// Compute initial parametric range of ray inside kd-tree extent
		Float tMin, tMax;
		if (!m_bounds.hit(ray, tMin, tMax)) 
		{
			return false;
		}

		// Prepare to traverse kd-tree for ray
		AVector3f invDir(1 / ray.m_dir.x, 1 / ray.m_dir.y, 1 / ray.m_dir.z);
		const int maxTodo = 64;
		AKdToDo todo[maxTodo];
		int todoPos = 0;

		// Traverse kd-tree nodes in order for ray
		bool hit = false;
		const AKdTreeNode *currNode = &m_nodes[0];
		while (currNode != nullptr)
		{
			// Bail out if we found a hit closer than the current node
			if (ray.m_tMax < tMin) 
				break;

			// Process kd-tree interior node
			if (!currNode->isLeaf()) 
			{
				// Compute parametric distance along ray to split plane
				int axis = currNode->splitAxis();
				Float tPlane = (currNode->splitPos() - ray.m_origin[axis]) * invDir[axis];

				// Get node children pointers for ray
				const AKdTreeNode *firstChild, *secondChild;
				int belowFirst = (ray.m_origin[axis] < currNode->splitPos()) ||
					(ray.m_origin[axis] == currNode->splitPos() && ray.m_dir[axis] <= 0);
				if (belowFirst) 
				{
					firstChild = currNode + 1;
					secondChild = &currNode[currNode->aboveChild()];
				}
				else 
				{
					firstChild = &currNode[currNode->aboveChild()];
					secondChild = currNode + 1;
				}

				// Advance to next child node, possibly enqueue other child
				if (tPlane > tMax || tPlane <= 0)
				{
					currNode = firstChild;
				}
				else if (tPlane < tMin)
				{
					currNode = secondChild;
				}
				else 
				{
					// Enqueue _secondChild_ in todo list
					todo[todoPos].node = secondChild;
					todo[todoPos].tMin = tPlane;
					todo[todoPos].tMax = tMax;
					++todoPos;
					currNode = firstChild;
					tMax = tPlane;
				}
			}
			else 
			{
				// Check for intersections inside leaf node
				int nHitables = currNode->numHitables();
				if (nHitables == 1)
				{
					const AHitable::ptr &p = m_hitables[currNode->m_oneHitable];
					// Check one hitable inside leaf node
					if (p->hit(ray, isect)) 
						hit = true;
				}
				else 
				{
					for (int i = 0; i < nHitables; ++i)
					{
						int index = m_hitableIndices[currNode->m_hitableIndicesOffset + i];
						const AHitable::ptr &p = m_hitables[index];
						// Check one hitable inside leaf node
						if (p->hit(ray, isect)) 
							hit = true;
					}
				}

				// Grab next node to process from todo list
				if (todoPos > 0) 
				{
					--todoPos;
					currNode = todo[todoPos].node;
					tMin = todo[todoPos].tMin;
					tMax = todo[todoPos].tMax;
				}
				else
				{
					break;
				}
			}

		}

		return hit;
	}

}