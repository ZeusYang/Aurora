#include "Material.h"

#include "ONB.h"
#include "Hitable.h"
#include "CosinePDF.h"

namespace Aurora
{
	bool Lambertian::scatter(const ARay &in, const HitRecord &rec, ScatterRecord &srec) const
	{
		srec.m_isSpecular = false;
		srec.m_attenuation = m_albedo;
		srec.m_pdf.reset(new CosinePDF(rec.m_normal));
		return true;
	}

	Float Lambertian::scattering_pdf(const ARay &in, const HitRecord &rec, const ARay &scattered) const
	{
		Float cosine = dot(rec.m_normal, scattered.direction());
		if (cosine < 0.0f)
			cosine = 0.0f;
		return cosine / aPi;
	}

	bool Metal::scatter(const ARay &in, const HitRecord &rec, ScatterRecord &srec) const
	{
		AVector3f reflectedDir = reflect(in.direction(), rec.m_normal);
		srec.m_scatterRay = ARay(rec.m_position, reflectedDir + randomInUnitSphere() * m_fuzz);
		srec.m_attenuation = m_albedo;
		srec.m_isSpecular = true;
		srec.m_pdf = nullptr;
		return true;
	}

	bool Dielectric::scatter(const ARay &in, const HitRecord &rec, ScatterRecord &srec) const
	{
		srec.m_isSpecular = true;
		srec.m_pdf = nullptr;
		AVector3f outward_normal;
		AVector3f reflected = reflect(in.direction(), rec.m_normal);
		Float ni_over_nt;
		srec.m_attenuation = ASpectrum(1.0f);
		AVector3f refracted;
		Float reflect_prob;
		Float cosine;
		// from inside to outside.
		if (dot(in.direction(), rec.m_normal) > 0.0f)
		{
			outward_normal = -rec.m_normal;
			ni_over_nt = refIdx;
			cosine = /*refIdx * */dot(in.direction(), rec.m_normal) / length(in.direction());
		}
		// from outside to inside.
		else
		{
			outward_normal = rec.m_normal;
			ni_over_nt = 1.0 / refIdx;
			// -1????
			cosine = dot(-in.direction(), rec.m_normal) / length(in.direction());
		}

		if (refract(-in.direction(), outward_normal, ni_over_nt, refracted))
		{
			reflect_prob = schlick(cosine, refIdx);
		}
		else
		{
			//scattered = Ray(rec.m_position, reflected);
			srec.m_scatterRay = ARay(rec.m_position, reflected);
			reflect_prob = 1.0f;
		}
		if (drand48() < reflect_prob)
			srec.m_scatterRay = ARay(rec.m_position, reflected);
		else
			srec.m_scatterRay = ARay(rec.m_position, refracted);
		return true;
	}

	bool DiffuseLight::scatter(const ARay & in, const HitRecord & rec, ScatterRecord & srec) const
	{
		return false;
		srec.m_isSpecular = false;
		srec.m_attenuation = m_albedo;
		srec.m_pdf.reset(new CosinePDF(rec.m_normal));
		return true;
	}

	ASpectrum DiffuseLight::emitted(const ARay &in, const HitRecord &rec, const Float & u,
		const Float & v, const AVector3f & p) const
	{
		return m_emitTex;
	}
}
