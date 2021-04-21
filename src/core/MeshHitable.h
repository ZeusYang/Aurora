#ifndef MESHHITABLE_H
#define MESHHITABLE_H

#include "Hitable.h"
#include "Transform3D.h"


namespace Aurora
{

	class MeshHitable : public Hitable
	{
	public:
		Transform3D m_transformation;
		std::vector<Vertex> m_vertices;
		std::vector<unsigned int> m_indices;
		std::vector<AVector3f> m_faceNormal;

		MeshHitable(const Material::ptr &mat) : Hitable(mat) {}
		virtual ~MeshHitable() = default;

		void setVertices(const std::vector<Vertex> &ver, const std::vector<unsigned int> &ind)
		{
			m_vertices = ver; m_indices = ind;
		}
		std::vector<Vertex> getVertices() const { return m_vertices; }
		std::vector<unsigned int> getIndices() const { return m_indices; }

		void scale(const AVector3f &ds) { m_transformation.scale(ds); }
		void translate(const AVector3f &dt) { m_transformation.translate(dt); }
		void rotate(const AVector3f &axis, Float angle) { m_transformation.rotate(axis, angle); }

		virtual void preRendering();
		virtual bool hit(const Ray &ray, const Float &t_min, const Float &t_max, HitRecord &ret) const;

	private:
		bool triangleHit(const Ray &ray, const Float &t_min, const Float &t_max,
			HitRecord &ret, const Vertex &p0, const Vertex &p1,
			const Vertex &p2, const AVector3f &normal) const;

	};

	class Plane : public MeshHitable
	{
	private:
		std::string m_name;

	public:
		Plane(const Material::ptr &mat, AVector3f pos, AVector3f len);
		virtual ~Plane() = default;

		void setName(std::string target) { m_name = target; }

		virtual Float pdfValue(const AVector3f &o, const AVector3f &v) const override;
		virtual AVector3f random(const AVector3f &o) const override;

		virtual std::string getName() const { return m_name; }

	};

}

#endif // MESHHITABLE_H
