#ifndef HITABLE_H
#define HITABLE_H

#include "Material.h"
#include "ArMathUtils.h"
#include <vector>


namespace Aurora
{

	struct HitRecord
	{
		Float m_t;
		AVector2f m_texcoord;
		AVector3f m_position;
		AVector3f m_normal;
		Material* m_material;
	};

	struct Vertex
	{
		AVector3f m_position;
		AVector3f m_normal;
		AVector2f m_texcoord;
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
		virtual std::string getName() const { return "Hitable"; }
	};

	class Sphere final : public Hitable
	{
	public:
		typedef std::shared_ptr<Sphere> ptr;

		Float m_radius;
		AVector3f m_center;

		Sphere(const Material::ptr &mat, const AVector3f &cen, const Float r)
			:Hitable(mat), m_center(cen), m_radius(r) {}
		virtual ~Sphere() = default;

		virtual bool hit(const ARay &ray, HitRecord &ret) const override;
		virtual Float pdfValue(const AVector3f &o, const AVector3f &v) const override;
		virtual AVector3f random(const AVector3f &o) const override;
	};

	class Triangle final : public Hitable
	{
	public:
		typedef std::shared_ptr<Triangle> ptr;

		AVector3f m_normal;
		AVector3f m_p0, m_p1, m_p2;

		Triangle(const Material::ptr &mat, AVector3f p0, AVector3f p1, AVector3f p2)
			: Hitable(mat), m_p0(p0), m_p1(p1), m_p2(p2)
		{
			m_normal = normalize(cross((p1 - p0), (p2 - p0)));
		}

		virtual ~Triangle() = default;

		virtual bool hit(const ARay &ray, HitRecord &ret) const override;

	};

	class HitableList : public Hitable
	{
	private:
		std::vector<Hitable::ptr> m_objects;

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
	};

}

#endif // HITABLE_H
