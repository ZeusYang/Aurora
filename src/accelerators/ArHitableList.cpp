#include "ArHitableList.h"

namespace Aurora
{
	ABounds3f ALinearAggregate::worldBound() const { return m_worldBounds; }

	void ALinearAggregate::addHitable(AHitable::ptr entity)
	{
		m_hitableList.push_back(entity);
		m_worldBounds = unionBounds(m_worldBounds, entity->worldBound());
	}

	bool ALinearAggregate::hit(const ARay &ray) const
	{
		for (int i = 0; i < m_hitableList.size(); i++)
		{
			if (m_hitableList[i]->hit(ray))
			{
				return true;
			}
		}
		return false;
	}

	bool ALinearAggregate::hit(const ARay &ray, ASurfaceInteraction &ret) const
	{
		ASurfaceInteraction temp_rec;
		bool hit_anything = false;
		for (int i = 0; i < m_hitableList.size(); i++)
		{
			if (m_hitableList[i]->hit(ray, temp_rec))
			{
				hit_anything = true;
				ret = temp_rec;
			}
		}
		return hit_anything;
	}
}