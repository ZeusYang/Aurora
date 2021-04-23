#ifndef ARHITABLE_H
#define ARHITABLE_H

#include "ArAurora.h"
#include "ArLight.h"
#include "ArShape.h"
#include "ArMaterial.h"

#include <memory>

namespace Aurora
{
	class AHitable
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

	};

	class AHitableEntity final : public AHitable
	{
	public:
		typedef std::shared_ptr<AHitableEntity> ptr;

		AHitableEntity(const AShape::ptr &shape, const AMaterial::ptr &material,
			const AAreaLight::ptr &areaLight);

		virtual bool hit(const ARay &ray) const override;
		virtual bool hit(const ARay &ray, ASurfaceInteraction &iset) const override;

		virtual ABounds3f worldBound() const override;

		virtual const AAreaLight *getAreaLight() const override;
		virtual const AMaterial *getMaterial() const override;

		virtual void computeScatteringFunctions(ASurfaceInteraction &isect, MemoryArena &arena,
			ATransportMode mode, bool allowMultipleLobes) const override;

	private:
		AShape::ptr m_shape;
		AMaterial::ptr m_material;
		AAreaLight::ptr m_areaLight;

	};

	class AHitableAggregate : public AHitable
	{
	public:

		virtual const AAreaLight *getAreaLight() const override;
		virtual const AMaterial *getMaterial() const override;

		virtual void computeScatteringFunctions(ASurfaceInteraction &isect, MemoryArena &arena,
			ATransportMode mode, bool allowMultipleLobes) const override;

	};

	class AHitableList final : public AHitableAggregate
	{
	public:
		typedef std::shared_ptr<AHitableList> ptr;

		AHitableList() = default;
		virtual bool hit(const ARay &ray) const override;
		virtual bool hit(const ARay &ray, ASurfaceInteraction &iset) const override;

		virtual ABounds3f worldBound() const override;

		bool isEmpty() const { return m_hitableList.empty(); }
		void addHitable(AHitable::ptr entity);

	private:
		using AHitableBuffer = std::vector<AHitable::ptr>;
		AHitableBuffer m_hitableList;
		ABounds3f m_worldBounds;
	};
}

#endif