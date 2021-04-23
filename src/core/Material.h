#ifndef MATERIAL_H
#define MATERIAL_H

#include "ArAurora.h"
#include "ArMathUtils.h"
#include "ArSpectrum.h"

namespace Aurora
{
	class PDF;
	class HitRecord;
	struct ScatterRecord
	{
		ARay m_scatterRay;
		bool m_isSpecular;
		ASpectrum m_attenuation;
		std::shared_ptr<PDF> m_pdf;
	};

	class Material
	{
	public:
		typedef std::shared_ptr<Material> ptr;

		Material() = default;
		virtual ~Material() = default;

		virtual bool scatter(const ARay &in, const HitRecord &rec, ScatterRecord &srec) const
		{
			return false;
		}

		virtual Float scattering_pdf(const ARay &in, const HitRecord &rec,
			const ARay &scattered) const
		{
			return 1.0f;
		}

		virtual ASpectrum emitted(const ARay &in, const HitRecord &rec, const Float &u,
			const Float &v, const AVector3f &p) const
		{
			return ASpectrum(0.0f);
		}
	};

	class Lambertian final : public Material
	{
	private:
		ASpectrum m_albedo;

	public:
		typedef std::shared_ptr<Lambertian> ptr;

		Lambertian(const ASpectrum &a) : m_albedo(a) {}
		virtual ~Lambertian() = default;

		virtual bool scatter(const ARay &in, const HitRecord &rec, ScatterRecord &srec) const override;

		virtual Float scattering_pdf(const ARay &in, const HitRecord &rec, const ARay &scattered) const override;

	};

	class DiffuseLight final : public Material
	{
	private:
		ASpectrum m_emitTex;
		ASpectrum m_albedo;

	public:
		typedef std::shared_ptr<DiffuseLight> ptr;

		DiffuseLight(const ASpectrum &a, const ASpectrum &b) : m_emitTex(a), m_albedo(b) { }

		virtual bool scatter(const ARay &in, const HitRecord &rec, ScatterRecord &srec) const override;

		virtual ASpectrum emitted(const ARay &in, const HitRecord &rec, const Float &u,
			const Float &v, const AVector3f &p) const;
	};

}

#endif // MATERIAL_H
