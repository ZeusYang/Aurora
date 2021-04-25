#include "ArHitable.h"

#include "ArInteraction.h"

namespace Aurora
{
	//-------------------------------------------AHitableEntity-------------------------------------

	AHitableEntity::AHitableEntity(const AShape::ptr &shape, const AMaterial::ptr &material,
		const AAreaLight::ptr &areaLight)
		: m_shape(shape), m_material(material), m_areaLight(areaLight) {}

	bool AHitableEntity::hit(const ARay &ray) const { return m_shape->hit(ray); }

	bool AHitableEntity::hit(const ARay &ray, ASurfaceInteraction &isect) const
	{
		Float tHit;
		if (!m_shape->hit(ray, tHit, isect))
			return false;

		ray.m_tMax = tHit;
		isect.hitable = this;
		return true;
	}

	void AHitableEntity::computeScatteringFunctions(ASurfaceInteraction &isect, MemoryArena &arena,
		ATransportMode mode, bool allowMultipleLobes) const
	{
		if (m_material != nullptr)
		{
			m_material->computeScatteringFunctions(isect, arena, mode, allowMultipleLobes);
		}
	}

	ABounds3f AHitableEntity::worldBound() const { return m_shape->worldBound(); }

	const AAreaLight* AHitableEntity::getAreaLight() const { return m_areaLight.get(); }

	const AMaterial* AHitableEntity::getMaterial() const { return m_material.get(); }

	//-------------------------------------------AHitableAggregate-------------------------------------

	const AAreaLight *AHitableAggregate::getAreaLight() const { return nullptr; }

	const AMaterial *AHitableAggregate::getMaterial() const { return nullptr; }

	void AHitableAggregate::computeScatteringFunctions(ASurfaceInteraction &isect, MemoryArena &arena,
		ATransportMode mode, bool allowMultipleLobes) const
	{
		//Note: should not go here at all.
		LOG(FATAL) <<
			"AHitableAggregate::computeScatteringFunctions() shouldn't be "
			"called";
	}

	//-------------------------------------------AHitableList-------------------------------------

	ABounds3f AHitableList::worldBound() const { return m_worldBounds; }

	void AHitableList::addHitable(AHitable::ptr entity)
	{
		m_hitableList.push_back(entity);
		m_worldBounds = unionBounds(m_worldBounds, entity->worldBound());
	}

	bool AHitableList::hit(const ARay &ray) const
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

	bool AHitableList::hit(const ARay &ray, ASurfaceInteraction &ret) const
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