#include "AABB.h"

namespace RayTracer
{
	std::vector<AABB> AABB::getEightSubAABB() const
	{
		Vector3D center = (m_min + m_max) * 0.5f;
		float edgeX = (m_max.x - m_min.x) * 0.5f;
		float edgeY = (m_max.y - m_min.y) * 0.5f;
		float edgeZ = (m_max.z - m_min.z) * 0.5f;

		std::vector<AABB> record;
		record.resize(8);

		record[0] = AABB(m_min, center);
		record[1] = AABB(m_min + Vector3D(edgeX, 0, 0), center + Vector3D(edgeX, 0, 0));
		record[2] = AABB(m_min + Vector3D(edgeX, 0, edgeZ), center + Vector3D(edgeX, 0, edgeZ));
		record[3] = AABB(m_min + Vector3D(0, 0, edgeZ), center + Vector3D(0, 0, edgeZ));
		
		record[4] = AABB(m_min + Vector3D(0, edgeY, 0), center + Vector3D(0, edgeY, 0));
		record[5] = AABB(m_min + Vector3D(edgeX, edgeY, 0), center + Vector3D(edgeX, edgeY, 0));
		record[6] = AABB(m_min + Vector3D(edgeX, edgeY, edgeZ), center + Vector3D(edgeX, edgeY, edgeZ));
		record[7] = AABB(m_min + Vector3D(0, edgeY, edgeZ), center + Vector3D(0, edgeY, edgeZ));

		return record;
	}
}
