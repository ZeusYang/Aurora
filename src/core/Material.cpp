#include "Material.h"

#include "Hitable.h"
#include "PDF.h"

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
