#ifndef MATERIAL_H
#define MATERIAL_H

#include "Ray.h"
#include "Hitable.h"
#include "Texture.h"
#include <memory>

/**
 * @projectName   RayTracer
 * @brief         Object's material class.
 * @author        YangWC
 * @date          2019-05-06
 */

namespace RayTracer
{
	class PDF;
	struct ScatterRecord
	{
		Ray m_scatterRay;
		bool m_isSpecular;
		Vector3D m_attenuation;
		std::shared_ptr<PDF> m_pdf;
	};

	class Material
	{
	public:
		typedef std::shared_ptr<Material> ptr;

		Material() = default;
		virtual ~Material() = default;

		virtual bool scatter(const Ray &in, const HitRecord &rec, ScatterRecord &srec) const
		{
			return false;
		}

		virtual float scattering_pdf(const Ray &in, const HitRecord &rec,
			const Ray &scattered) const
		{
			return 1.0f;
		}

		virtual Vector3D emitted(const Ray &in, const HitRecord &rec, const float &u,
			const float &v, const Vector3D &p) const
		{
			return Vector3D(0.0f, 0.0f, 0.0f);
		}
	};

	class Lambertian : public Material
	{
	private:
		unsigned int m_albedo;

	public:
		typedef std::shared_ptr<Lambertian> ptr;

		Lambertian(unsigned int a) : m_albedo(a) {}
		virtual ~Lambertian() = default;

		virtual bool scatter(const Ray &in, const HitRecord &rec, ScatterRecord &srec) const;

		virtual float scattering_pdf(const Ray &in, const HitRecord &rec,
			const Ray &scattered) const;

	};

	class Metal : public Material
	{
	private:
		float m_fuzz;
		unsigned int m_albedo;

	public:
		typedef std::shared_ptr<Metal> ptr;

		Metal(unsigned int a, const float &f) : m_albedo(a), m_fuzz(f)
		{
			if (f > 1.0f)m_fuzz = 1.0f;
		}
		virtual ~Metal() = default;

		virtual bool scatter(const Ray &in, const HitRecord &rec, ScatterRecord &srec) const;
	};

	class Dielectric : public Material
	{
	private:
		float refIdx;

		float schlick(float cosine, float ref_idx) const
		{
			float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
			r0 = r0 * r0;
			return r0 + (1.0f - r0) * pow((1.0f - cosine), 5.0f);
		}

	public:
		typedef std::shared_ptr<Dielectric> ptr;

		Dielectric(float ri) : refIdx(ri) {}
		virtual ~Dielectric() = default;

		virtual bool scatter(const Ray &in, const HitRecord &rec, ScatterRecord &srec) const;
	};

	class DiffuseLight : public Material
	{
	private:
		unsigned int m_emitTex;
		unsigned int m_albedo;

	public:
		typedef std::shared_ptr<DiffuseLight> ptr;

		DiffuseLight(unsigned int a, unsigned int b) : m_emitTex(a), m_albedo(b) { }

		virtual bool scatter(const Ray &in, const HitRecord &rec, ScatterRecord &srec) const;

		virtual Vector3D emitted(const Ray &in, const HitRecord &rec, const float &u,
			const float &v, const Vector3D &p) const;
	};

}

#endif // MATERIAL_H
