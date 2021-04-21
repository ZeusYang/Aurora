#ifndef MATERIAL_H
#define MATERIAL_H

#include "Ray.h"
#include "Texture.h"
#include <memory>

namespace Aurora
{
	class PDF;
	class HitRecord;
	struct ScatterRecord
	{
		Ray m_scatterRay;
		bool m_isSpecular;
		AVector3f m_attenuation;
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

		virtual Float scattering_pdf(const Ray &in, const HitRecord &rec,
			const Ray &scattered) const
		{
			return 1.0f;
		}

		virtual AVector3f emitted(const Ray &in, const HitRecord &rec, const Float &u,
			const Float &v, const AVector3f &p) const
		{
			return AVector3f(0.0f, 0.0f, 0.0f);
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

		virtual Float scattering_pdf(const Ray &in, const HitRecord &rec,
			const Ray &scattered) const;

	};

	class Metal : public Material
	{
	private:
		Float m_fuzz;
		unsigned int m_albedo;

	public:
		typedef std::shared_ptr<Metal> ptr;

		Metal(unsigned int a, const Float &f) : m_albedo(a), m_fuzz(f)
		{
			if (f > 1.0f)m_fuzz = 1.0f;
		}
		virtual ~Metal() = default;

		virtual bool scatter(const Ray &in, const HitRecord &rec, ScatterRecord &srec) const;
	};

	class Dielectric : public Material
	{
	private:
		Float refIdx;

		Float schlick(Float cosine, Float ref_idx) const
		{
			Float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
			r0 = r0 * r0;
			return r0 + (1.0f - r0) * pow((1.0f - cosine), 5.0f);
		}

	public:
		typedef std::shared_ptr<Dielectric> ptr;

		Dielectric(Float ri) : refIdx(ri) {}
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

		virtual AVector3f emitted(const Ray &in, const HitRecord &rec, const Float &u,
			const Float &v, const AVector3f &p) const;
	};

}

#endif // MATERIAL_H
