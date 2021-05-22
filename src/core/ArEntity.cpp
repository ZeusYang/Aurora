#include "ArEntity.h"

#include "ArShape.h"

namespace Aurora
{
	AURORA_REGISTER_CLASS(AEntity, "Entity")

	AEntity::AEntity(const APropertyTreeNode &node)
	{
		const APropertyList& props = node.getPropertyList();

		// Shape
		const auto &shapeNode = node.getPropertyChild("Shape");
		AShape::ptr shape = AShape::ptr(static_cast<AShape*>(AObjectFactory::createInstance(
			shapeNode.getTypeName(), shapeNode)));
		shape->setTransform(&m_objectToWorld, &m_worldToObject);

		// Transform
		const auto &shapeProps = shapeNode.getPropertyList();
		AVector3f _trans = shapeProps.getVector3f("Translate", AVector3f(0.0f));
		AVector3f _scale = shapeProps.getVector3f("Scale", AVector3f(1.0f));
		m_objectToWorld = translate(_trans) * scale(_scale.x, _scale.y, _scale.z);
		m_worldToObject = inverse(m_objectToWorld);

		// Material
		const auto &materialNode = node.getPropertyChild("Material");
		m_material = AMaterial::ptr(static_cast<AMaterial*>(AObjectFactory::createInstance(
				materialNode.getTypeName(), materialNode)));

		//Area light
		AAreaLight::ptr areaLight = nullptr;
		if (node.hasPropertyChild("Light"))
		{
			const auto &lightNode = node.getPropertyChild("Light");
			areaLight = AAreaLight::ptr(static_cast<AAreaLight*>(AObjectFactory::createInstance(
				lightNode.getTypeName(), lightNode)));
		}

		m_hitables.push_back(std::make_shared<AHitableObject>(shape, m_material.get(), areaLight));
	}


	AURORA_REGISTER_CLASS(AMeshEntity, "MeshEntity")

	AMeshEntity::AMeshEntity(const APropertyTreeNode &node)
	{
		const APropertyList& props = node.getPropertyList();
		const std::string filename = props.getString("Filename");

		// Shape
		const auto &shapeNode = node.getPropertyChild("Shape");

		// Transform
		const auto &shapeProps = shapeNode.getPropertyList();
		AVector3f _trans = shapeProps.getVector3f("Translate", AVector3f(0.0f));
		AVector3f _scale = shapeProps.getVector3f("Scale", AVector3f(1.0f));
		m_objectToWorld = translate(_trans) * scale(_scale.x, _scale.y, _scale.z);
		m_worldToObject = inverse(m_objectToWorld);

		//Material
		const auto &materialNode = node.getPropertyChild("Material");
		m_material = AMaterial::ptr(static_cast<AMaterial*>(AObjectFactory::createInstance(
			materialNode.getTypeName(), materialNode)));

		//Load each triangle of the mesh as a HitableEntity
		m_mesh = ATriangleMesh::unique_ptr(new ATriangleMesh(&m_objectToWorld, APropertyTreeNode::m_directory + filename));
		const auto &meshIndices = m_mesh->getIndices();
		for (size_t i = 0; i < meshIndices.size(); i += 3)
		{
			std::array<int, 3> indices;
			indices[0] = meshIndices[i + 0];
			indices[1] = meshIndices[i + 1];
			indices[2] = meshIndices[i + 2];
			ATriangleShape::ptr triangle = std::make_shared<ATriangleShape>(&m_objectToWorld, &m_worldToObject, indices, m_mesh.get());

			//Area light
			AAreaLight::ptr areaLight = nullptr;
			if (node.hasPropertyChild("Light"))
			{
				const auto &lightNode = node.getPropertyChild("Light");
				areaLight = AAreaLight::ptr(static_cast<AAreaLight*>(AObjectFactory::createInstance(
					lightNode.getTypeName(), lightNode)));
			}
			m_hitables.push_back(std::make_shared<AHitableObject>(triangle, m_material.get(), areaLight));
		}
	}

}