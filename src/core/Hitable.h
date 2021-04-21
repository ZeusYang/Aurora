#ifndef HITABLE_H
#define HITABLE_H

#include "Ray.h"
#include "AABB.h"
#include "Vector2D.h"
#include <vector>

/**
 * @projectName   RayTracer
 * @brief         An object that could be hit by a given ray.
 * @author        YangWC
 * @date          2019-05-06
 */

namespace RayTracer
{

	class Material;
	struct HitRecord
	{
		float m_t;
		Vector2D m_texcoord;
		Vector3D m_position;
		Vector3D m_normal;
		unsigned int m_material;
	};

	struct Vertex
	{
		Vector3D m_position;
		Vector3D m_normal;
		Vector2D m_texcoord;
	};

	class Hitable
	{
	public:
		Hitable() = default;
		virtual ~Hitable() = default;
		virtual bool isLeaf() const { return true; }
		virtual void preRendering() {}
		virtual bool hit(const Ray &ray, const float &t_min, const float &t_max, HitRecord &ret) const = 0;
		virtual bool boundingBox(const float &t0, const float &t1, AABB &box) const = 0;
		virtual float pdfValue(const Vector3D &o, const Vector3D &v) const { return 0.0f; }
		virtual Vector3D random(const Vector3D &o) const { return Vector3D(1.0f, 0.0f, 0.0f); }
		virtual std::string getName() const { return "Hitable"; }
	};

	class Sphere : public Hitable
	{
	public:
		float m_radius;
		Vector3D m_center;
		unsigned int m_material;

		Sphere(const Vector3D &cen, const float r, unsigned int mat)
			:m_center(cen), m_radius(r), m_material(mat) {}
		virtual ~Sphere() = default;

		virtual bool hit(const Ray &ray, const float &t_min, const float &t_max, HitRecord &ret) const;
		virtual bool boundingBox(const float &t0, const float &t1, AABB &box) const;
		virtual float pdfValue(const Vector3D &o, const Vector3D &v) const;
		virtual Vector3D random(const Vector3D &o) const;
	};

	class TTriangle : public Hitable
	{
	public:
		Vector3D m_normal;
		Vector3D m_p0, m_p1, m_p2;
		unsigned int m_material;

		TTriangle(Vector3D p0, Vector3D p1, Vector3D p2, unsigned int mat)
			:m_p0(p0), m_p1(p1), m_p2(p2), m_material(mat)
		{
			m_normal = (p1 - p0).crossProduct(p2 - p0);
			m_normal.normalize();
		}
		virtual ~TTriangle() = default;

		virtual bool hit(const Ray &ray, const float &t_min, const float &t_max, HitRecord &ret) const;
		virtual bool boundingBox(const float &t0, const float &t1, AABB &box) const;

	};

	class HitableList : public Hitable
	{
	private:
		std::vector<Hitable*> m_list;

	public:
		HitableList() = default;
		virtual bool hit(const Ray &ray, const float &t_min, const float &t_max, HitRecord &ret) const;
		virtual bool boundingBox(const float &t0, const float &t1, AABB &box) const;
		virtual float pdfValue(const Vector3D &o, const Vector3D &v) const;
		virtual Vector3D random(const Vector3D &o) const;

		bool isEmpty() const { return m_list.empty(); }
		void addObjects(Hitable *target) { m_list.push_back(target); }
	};

}

#endif // HITABLE_H
