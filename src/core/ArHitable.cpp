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
		: m_shape(shape), m_material(material), m_areaLight(areaLight) 
	{
		if (m_areaLight != nullptr)
		{
			m_areaLight->setParent(this);
		}
	}

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
		const APropertyList& props = node.getPropertyList();
		const std::string filename = props.getString("Filename");
		
		//Shape
		ATransform objectToWorld, worldToObject;
		{
			const auto &shapeNode = node.getPropertyChild("Shape");
			const auto &shapeProps = shapeNode.getPropertyList();
			AVector3f _trans = shapeProps.getVector3f("Translate", AVector3f(0.0f));
			AVector3f _scale = shapeProps.getVector3f("Scale", AVector3f(1.0f));
			objectToWorld = translate(_trans) * scale(_scale.x, _scale.y, _scale.z);
			worldToObject = inverse(objectToWorld);
		}

		//Material
		{
			const auto &materialNode = node.getPropertyChild("Material");
			m_material = AMaterial::ptr(static_cast<AMaterial*>(AObjectFactory::createInstance(
				materialNode.getTypeName(), materialNode)));
		}

		//Load each triangle of the mesh as a HitableEntity
		m_mesh = std::make_shared<ATriangleMesh>(objectToWorld, APropertyTreeNode::m_directory + filename);
		const auto &meshIndices = m_mesh->getIndices();
		for (size_t i = 0; i < meshIndices.size(); i += 3)
		{
			std::array<int, 3> indices;
			indices[0] = meshIndices[i + 0];
			indices[1] = meshIndices[i + 1];
			indices[2] = meshIndices[i + 2];
			ATriangleShape::ptr triangle = std::make_shared<ATriangleShape>(objectToWorld, worldToObject, indices, m_mesh.get());

			//Area light
			AAreaLight::ptr areaLight = nullptr;
			if (node.hasPropertyChild("Light"))
			{
				const auto &lightNode = node.getPropertyChild("Light");
				areaLight = AAreaLight::ptr(static_cast<AAreaLight*>(AObjectFactory::createInstance(
					lightNode.getTypeName(), lightNode)));
			}
			m_triangles.push_back(std::make_shared<AHitableEntity>(triangle, m_material, areaLight));
		}
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
		ABounds3f bound;
		for (const auto &tri : m_triangles)
		{
			bound = unionBounds(bound, tri->worldBound());
		}
		return bound;
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