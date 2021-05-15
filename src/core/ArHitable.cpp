#include "ArHitable.h"

#include "ArInteraction.h"

namespace Aurora
{
	//-------------------------------------------AHitableObject-------------------------------------

	AHitableObject::AHitableObject(const AShape::ptr &shape, const AMaterial* material,
		const AAreaLight::ptr &areaLight)
		: m_shape(shape), m_material(material), m_areaLight(areaLight) 
	{
		if (m_areaLight != nullptr)
		{
			m_areaLight->setParent(this);
		}
	}

	bool AHitableObject::hit(const ARay &ray) const { return m_shape->hit(ray); }

	bool AHitableObject::hit(const ARay &ray, ASurfaceInteraction &isect) const
	{
		Float tHit;
		if (!m_shape->hit(ray, tHit, isect))
			return false;

		ray.m_tMax = tHit;
		isect.hitable = this;
		return true;
	}

	void AHitableObject::computeScatteringFunctions(ASurfaceInteraction &isect, MemoryArena &arena,
		ATransportMode mode, bool allowMultipleLobes) const
	{
		if (m_material != nullptr)
		{
			m_material->computeScatteringFunctions(isect, arena, mode, allowMultipleLobes);
		}
	}

	ABounds3f AHitableObject::worldBound() const { return m_shape->worldBound(); }

	AShape* AHitableObject::getShape() const { return m_shape.get(); }

	const AAreaLight* AHitableObject::getAreaLight() const { return m_areaLight.get(); }

	const AMaterial* AHitableObject::getMaterial() const { return m_material; }

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

}