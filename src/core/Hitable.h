#ifndef HITABLE_H
#define HITABLE_H

#include "Ray.h"
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
		Material::ptr m_material;

		Hitable(const Material::ptr &mat) : m_material(mat) {}
		virtual ~Hitable() = default;
		virtual bool isLeaf() const { return true; }
		virtual void preRendering() {}

		virtual bool hit(const Ray &ray, const Float &t_min, const Float &t_max, HitRecord &ret) const = 0;

		virtual Float pdfValue(const AVector3f &o, const AVector3f &v) const { return 0.0f; }

		virtual AVector3f random(const AVector3f &o) const { return AVector3f(1.0f, 0.0f, 0.0f); }
		virtual std::string getName() const { return "Hitable"; }
	};

	class Sphere final : public Hitable
	{
	public:
		Float m_radius;
		AVector3f m_center;

		Sphere(const Material::ptr &mat, const AVector3f &cen, const Float r)
			:Hitable(mat), m_center(cen), m_radius(r) {}
		virtual ~Sphere() = default;

		virtual bool hit(const Ray &ray, const Float &t_min, const Float &t_max, HitRecord &ret) const override;
		virtual Float pdfValue(const AVector3f &o, const AVector3f &v) const override;
		virtual AVector3f random(const AVector3f &o) const override;
	};

	class TTriangle final : public Hitable
	{
	public:
		AVector3f m_normal;
		AVector3f m_p0, m_p1, m_p2;

		TTriangle(const Material::ptr &mat, AVector3f p0, AVector3f p1, AVector3f p2)
			: Hitable(mat), m_p0(p0), m_p1(p1), m_p2(p2)
		{
			m_normal = normalize(cross((p1 - p0), (p2 - p0)));
		}
		virtual ~TTriangle() = default;

		virtual bool hit(const Ray &ray, const Float &t_min, const Float &t_max, HitRecord &ret) const override;

	};

	class HitableList : public Hitable
	{
	private:
		std::vector<Hitable*> m_list;

	public:
		HitableList() : Hitable(nullptr) {}
		virtual bool hit(const Ray &ray, const Float &t_min, const Float &t_max, HitRecord &ret) const override;
		virtual Float pdfValue(const AVector3f &o, const AVector3f &v) const override;
		virtual AVector3f random(const AVector3f &o) const override;

		virtual void preRendering() override 
		{
			for (int x = 0; x < m_list.size(); ++x)
			{
				m_list[x]->preRendering();
			}
		}


		bool isEmpty() const { return m_list.empty(); }
		void addObjects(Hitable *target) { m_list.push_back(target); }
	};

}

#endif // HITABLE_H
