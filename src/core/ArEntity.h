#ifndef AENTITY_H
#define AENTITY_H

#include "ArAurora.h"
#include "ArMathUtils.h"
#include "ArRtti.h"
#include "ArHitable.h"
#include "ArTriangleShape.h"

namespace Aurora
{
	//! @brief Basic unit of objects in world.
	/**
	 * An entity is the very basic concept in a world. Everything, including camera, mesh, light or anything else is an
	 * entity. An entity could parse itself and decouple itself into one or multiple primitives depending its complexity.
	 * An entity itself doesn't touch rendering directly. It serves as a place where the logic operations should be performed.
	 */
	class AEntity : public AObject
	{
	public:
		typedef std::shared_ptr<AEntity> ptr;

		AEntity() = default;
		AEntity(const APropertyTreeNode &node);

		AMaterial* getMaterial() const { return m_material.get(); }
		const std::vector<AHitable::ptr>& getHitables() const { return m_hitables; }

		virtual std::string toString() const override { return "Entity[]"; }
		virtual AClassType getClassType() const override { return AClassType::AEHitable; }

	protected:
		AMaterial::ptr m_material;
		std::vector<AHitable::ptr> m_hitables;
		ATransform m_objectToWorld, m_worldToObject;

	};

	class AMeshEntity : public AEntity
	{
	public:
		typedef std::shared_ptr<AMeshEntity> ptr;

		AMeshEntity(const APropertyTreeNode &node);

		virtual std::string toString() const override { return "MeshEntity[]"; }

	private:
		ATriangleMesh::unique_ptr m_mesh;
	};

}


#endif