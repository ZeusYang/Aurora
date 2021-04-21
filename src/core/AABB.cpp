#include "AABB.h"

namespace Aurora
{
	std::vector<AABB> AABB::getEightSubAABB() const
	{
		AVector3f center = (m_min + m_max) * 0.5f;
		Float edgeX = (m_max.x - m_min.x) * 0.5f;
		Float edgeY = (m_max.y - m_min.y) * 0.5f;
		Float edgeZ = (m_max.z - m_min.z) * 0.5f;

		std::vector<AABB> record;
		record.resize(8);

		record[0] = AABB(m_min, center);
		record[1] = AABB(m_min + AVector3f(edgeX, 0, 0), center + AVector3f(edgeX, 0, 0));
		record[2] = AABB(m_min + AVector3f(edgeX, 0, edgeZ), center + AVector3f(edgeX, 0, edgeZ));
		record[3] = AABB(m_min + AVector3f(0, 0, edgeZ), center + AVector3f(0, 0, edgeZ));
		
		record[4] = AABB(m_min + AVector3f(0, edgeY, 0), center + AVector3f(0, edgeY, 0));
		record[5] = AABB(m_min + AVector3f(edgeX, edgeY, 0), center + AVector3f(edgeX, edgeY, 0));
		record[6] = AABB(m_min + AVector3f(edgeX, edgeY, edgeZ), center + AVector3f(edgeX, edgeY, edgeZ));
		record[7] = AABB(m_min + AVector3f(0, edgeY, edgeZ), center + AVector3f(0, edgeY, edgeZ));

		return record;
	}
}
