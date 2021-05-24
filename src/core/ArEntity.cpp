#include "ArEntity.h"

#include "ArShape.h"

namespace Aurora
{
	//-------------------------------------------AEntity-------------------------------------

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
		ATransform objectToWrold;
		const auto &shapeProps = shapeNode.getPropertyList();
		if (shapeNode.hasProperty("Transform"))
		{
			std::vector<ATransform> transformStack;
			std::vector<Float> sequence = shapeProps.getVectorNf("Transform");
			size_t it = 0;
			bool undefined = false;
			while (it < sequence.size() && !undefined)
			{
				int token = static_cast<int>(sequence[it]);
				switch (token)
				{
				case 0://translate
					CHECK_LT(it + 3, sequence.size());
					AVector3f _trans = AVector3f(sequence[it + 1], sequence[it + 2], sequence[it + 3]);
					transformStack.push_back(translate(_trans));
					it += 4;
					break;
				case 1://scale
					CHECK_LT(it + 3, sequence.size());
					AVector3f _scale = AVector3f(sequence[it + 1], sequence[it + 2], sequence[it + 3]);
					transformStack.push_back(scale(_scale.x, _scale.y, _scale.z));
					it += 4;
					break;
				case 2://rotate
					CHECK_LT(it + 4, sequence.size());
					AVector3f axis = AVector3f(sequence[it + 1], sequence[it + 2], sequence[it + 3]);
					transformStack.push_back(rotate(sequence[it + 4], axis));
					it += 5;
					break;
				default:
					undefined = true;
					LOG(ERROR) << "Undefined transform action";
					break;
				}
			}

			//Note: calculate the transform matrix in a first-in-last-out manner
			if (!undefined)
			{
				for (auto it = transformStack.rbegin(); it != transformStack.rend(); ++it)
				{
					objectToWrold = objectToWrold * (*it);
				}
			}
		}
		m_objectToWorld = objectToWrold;
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

	//-------------------------------------------AMeshEntity-------------------------------------

	AURORA_REGISTER_CLASS(AMeshEntity, "MeshEntity")

	AMeshEntity::AMeshEntity(const APropertyTreeNode &node)
	{
		const APropertyList& props = node.getPropertyList();
		const std::string filename = props.getString("Filename");

		// Shape
		const auto &shapeNode = node.getPropertyChild("Shape");

		// Transform
		ATransform objectToWrold;
		const auto &shapeProps = shapeNode.getPropertyList();
		if (shapeNode.hasProperty("Transform"))
		{
			std::vector<ATransform> transformStack;
			std::vector<Float> sequence = shapeProps.getVectorNf("Transform");
			size_t it = 0;
			bool undefined = false;
			while (it < sequence.size() && !undefined)
			{
				int token = static_cast<int>(sequence[it]);
				switch (token)
				{
				case 0://translate
					CHECK_LT(it + 3, sequence.size());
					AVector3f _trans = AVector3f(sequence[it + 1], sequence[it + 2], sequence[it + 3]);
					transformStack.push_back(translate(_trans));
					it += 4;
					break;
				case 1://scale
					CHECK_LT(it + 3, sequence.size());
					AVector3f _scale = AVector3f(sequence[it + 1], sequence[it + 2], sequence[it + 3]);
					transformStack.push_back(scale(_scale.x, _scale.y, _scale.z));
					it += 4;
					break;
				case 2://rotate
					CHECK_LT(it + 4, sequence.size());
					AVector3f axis = AVector3f(sequence[it + 1], sequence[it + 2], sequence[it + 3]);
					transformStack.push_back(rotate(sequence[it + 4], axis));
					it += 5;
					break;
				default:
					undefined = true;
					LOG(ERROR) << "Undefined transform action";
					break;
				}
			}

			//Note: calculate the transform matrix in a first-in-last-out manner
			if (!undefined)
			{
				for (auto it = transformStack.rbegin(); it != transformStack.rend(); ++it)
				{
					objectToWrold = objectToWrold * (*it);
				}
			}
		}
		m_objectToWorld = objectToWrold;
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