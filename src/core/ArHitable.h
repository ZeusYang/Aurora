#ifndef ARHITABLE_H
#define ARHITABLE_H

#include "ArAurora.h"
#include "ArLight.h"
#include "ArShape.h"
#include "ArRtti.h"
#include "ArMaterial.h"

#include <memory>

namespace Aurora
{
	class AHitable : public AObject
	{
	public:
		typedef std::shared_ptr<AHitable> ptr;

		virtual ~AHitable() = default;

		virtual bool hit(const ARay &ray) const = 0;
		virtual bool hit(const ARay &ray, ASurfaceInteraction &iset) const = 0;

		virtual ABounds3f worldBound() const = 0;

		virtual const AAreaLight *getAreaLight() const = 0;
		virtual const AMaterial *getMaterial() const = 0;

		virtual void computeScatteringFunctions(ASurfaceInteraction &isect, MemoryArena &arena,
			ATransportMode mode, bool allowMultipleLobes) const = 0;

		virtual AClassType getClassType() const override { return AClassType::AEHitable; }

	};

	class AHitableObject final : public AHitable
	{
	public:
		typedef std::shared_ptr<AHitableObject> ptr;

		AHitableObject(const AShape::ptr &shape, const AMaterial* material,
			const AAreaLight::ptr &areaLight);

		virtual bool hit(const ARay &ray) const override;
		virtual bool hit(const ARay &ray, ASurfaceInteraction &iset) const override;

		virtual ABounds3f worldBound() const override;

		AShape* getShape() const;
		AAreaLight::ptr getAreaLightPtr() const { return m_areaLight; }
		virtual const AAreaLight *getAreaLight() const override;
		virtual const AMaterial *getMaterial() const override;

		virtual void computeScatteringFunctions(ASurfaceInteraction &isect, MemoryArena &arena,
			ATransportMode mode, bool allowMultipleLobes) const override;

		virtual std::string toString() const override { return "HitableObject[]"; }

	private:
		AShape::ptr m_shape;
		AAreaLight::ptr m_areaLight;

		const AMaterial* m_material;
	};

	class AHitableAggregate : public AHitable
	{
	public:

		virtual const AAreaLight *getAreaLight() const override;
		virtual const AMaterial *getMaterial() const override;

		virtual void computeScatteringFunctions(ASurfaceInteraction &isect, MemoryArena &arena,
			ATransportMode mode, bool allowMultipleLobes) const override;

	};
}

#endif