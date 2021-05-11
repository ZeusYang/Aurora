#include "ArHitable.h"

#include "ArInteraction.h"

namespace Aurora
{
	//-------------------------------------------AHitableEntity-------------------------------------

	AURORA_REGISTER_CLASS(AHitableEntity, "Entity")

	AHitableEntity::AHitableEntity(const APropertyTreeNode &node)
	{
		//Shape
		{
			const auto &shapeNode = node.getPropertyChild("Shape");
			m_shape = AShape::ptr(static_cast<AShape*>(AObjectFactory::createInstance(
				shapeNode.getTypeName(), shapeNode)));
		}

		//Material
		{
			const auto &materialNode = node.getPropertyChild("Material");
			m_material = AMaterial::ptr(static_cast<AMaterial*>(AObjectFactory::createInstance(
				materialNode.getTypeName(), materialNode)));
		}

		//AreaLight
		{
			if (node.hasPropertyChild("Light"))
			{
				const auto &lightNode = node.getPropertyChild("Light");
				m_areaLight = AAreaLight::ptr(static_cast<AAreaLight*>(AObjectFactory::createInstance(
					lightNode.getTypeName(), lightNode)));
			}
			else
			{
				m_areaLight = nullptr;
			}
		}

		if (m_areaLight != nullptr)
		{
			m_areaLight->setParent(this);
		}

		activate();
	}

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

	AShape* AHitableEntity::getShape() const { return m_shape.get(); }

	const AAreaLight* AHitableEntity::getAreaLight() const { return m_areaLight.get(); }

	const AMaterial* AHitableEntity::getMaterial() const { return m_material.get(); }

	//-------------------------------------------AHitableMesh-------------------------------------

	AURORA_REGISTER_CLASS(AHitableMesh, "MeshEntity")

	AHitableMesh::AHitableMesh(const APropertyTreeNode &node)
	{
		//TODO: implement mesh loading
	}

	bool AHitableMesh::hit(const ARay &ray) const
	{
		return false;
	}

	bool AHitableMesh::hit(const ARay &ray, ASurfaceInteraction &iset) const
	{
		return false;
	}

	ABounds3f AHitableMesh::worldBound() const
	{
		return ABounds3f();
	}
	
	const AAreaLight* AHitableMesh::getAreaLight() const 
	{ 
		//Note: should not go here at all.
		LOG(FATAL) << "AHitableMesh::getAreaLight() shouldn't be called";
		return nullptr;
	}

	const AMaterial* AHitableMesh::getMaterial() const { return m_material.get(); }

	void AHitableMesh::computeScatteringFunctions(ASurfaceInteraction &isect, MemoryArena &arena,
		ATransportMode mode, bool allowMultipleLobes) const
	{
		LOG(FATAL) << "AHitableMesh::computeScatteringFunctions() shouldn't be " 
			"called";
	}

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