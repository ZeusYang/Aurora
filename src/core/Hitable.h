#ifndef HITABLE_H
#define HITABLE_H

#include "Material.h"
#include "ArShape.h"
#include "ArMathUtils.h"
#include <vector>

namespace Aurora
{

	struct HitRecord
	{
		AVector2f m_texcoord;
		AVector3f m_position;
		AVector3f m_normal;
		Material* m_material;
	};

	class Hitable
	{
	public:
		typedef std::shared_ptr<Hitable> ptr;
		
		Material::ptr m_material;

		Hitable(const Material::ptr &mat) : m_material(mat) {}
		virtual ~Hitable() = default;
		virtual bool isLeaf() const { return true; }
		virtual void preRendering() {}

		virtual bool hit(const ARay &ray, HitRecord &ret) const = 0;

		virtual Float pdfValue(const AVector3f &o, const AVector3f &v) const { return 0.0f; }

		virtual AVector3f random(const AVector3f &o) const { return AVector3f(1.0f, 0.0f, 0.0f); }
	};

	class HitableEntity final : public Hitable
	{
	public:
		typedef std::shared_ptr<HitableEntity> ptr;

		HitableEntity(const Material::ptr &mat, const AShape::ptr &shape)
			: Hitable(mat), m_shape(shape) {}

		virtual ~HitableEntity() = default;

		virtual bool hit(const ARay &ray, HitRecord &ret) const override;

	private:
		AShape::ptr m_shape;

	};

	class HitableList : public Hitable
	{
	public:
		typedef std::shared_ptr<HitableList> ptr;

		HitableList() : Hitable(nullptr) {}
		virtual bool hit(const ARay &ray, HitRecord &ret) const override;
		virtual Float pdfValue(const AVector3f &o, const AVector3f &v) const override;
		virtual AVector3f random(const AVector3f &o) const override;

		virtual void preRendering() override 
		{
			for (int x = 0; x < m_objects.size(); ++x)
			{
				m_objects[x]->preRendering();
			}
		}

		bool isEmpty() const { return m_objects.empty(); }
		void addObjects(const Hitable::ptr &object) { m_objects.push_back(object); }

	private:
		std::vector<Hitable::ptr> m_objects;
	};

}

#endif // HITABLE_H
