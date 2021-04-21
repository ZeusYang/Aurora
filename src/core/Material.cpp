#include "Material.h"

#include "TextureMgr.h"
#include "ONB.h"
#include "Hitable.h"
#include "CosinePDF.h"

namespace Aurora
{
	bool Lambertian::scatter(const Ray &in, const HitRecord &rec, ScatterRecord &srec) const
	{
		srec.m_isSpecular = false;
		srec.m_attenuation = TextureMgr::getSingleton()->getTexture(m_albedo)
			->sample(rec.m_texcoord.x, rec.m_texcoord.y, rec.m_position);
		srec.m_pdf.reset(new CosinePDF(rec.m_normal));
		return true;
	}

	Float Lambertian::scattering_pdf(const Ray &in, const HitRecord &rec, const Ray &scattered) const
	{
		Float cosine = dot(rec.m_normal, scattered.getDirection());
		if (cosine < 0.0f)
			cosine = 0.0f;
		return cosine / aPi;
	}

	bool Metal::scatter(const Ray &in, const HitRecord &rec, ScatterRecord &srec) const
	{
		AVector3f reflectedDir = reflect(in.getDirection(), rec.m_normal);
		srec.m_scatterRay = Ray(rec.m_position, reflectedDir + randomInUnitSphere() * m_fuzz);
		srec.m_attenuation = TextureMgr::getSingleton()->getTexture(m_albedo)
			->sample(rec.m_texcoord.x, rec.m_texcoord.y, rec.m_position);
		srec.m_isSpecular = true;
		srec.m_pdf = nullptr;
		return true;
	}

	bool Dielectric::scatter(const Ray &in, const HitRecord &rec, ScatterRecord &srec) const
	{
		srec.m_isSpecular = true;
		srec.m_pdf = nullptr;
		AVector3f outward_normal;
		AVector3f reflected = reflect(in.getDirection(), rec.m_normal);
		Float ni_over_nt;
		srec.m_attenuation = AVector3f(1.0f, 1.0f, 1.0f);
		AVector3f refracted;
		Float reflect_prob;
		Float cosine;
		// from inside to outside.
		if (dot(in.getDirection(), rec.m_normal) > 0.0f)
		{
			outward_normal = -rec.m_normal;
			ni_over_nt = refIdx;
			cosine = /*refIdx * */dot(in.getDirection(), rec.m_normal) / length(in.getDirection());
		}
		// from outside to inside.
		else
		{
			outward_normal = rec.m_normal;
			ni_over_nt = 1.0 / refIdx;
			// -1????
			cosine = dot(-in.getDirection(), rec.m_normal) / length(in.getDirection());
		}

		if (refract(-in.getDirection(), outward_normal, ni_over_nt, refracted))
		{
			reflect_prob = schlick(cosine, refIdx);
		}
		else
		{
			//scattered = Ray(rec.m_position, reflected);
			srec.m_scatterRay = Ray(rec.m_position, reflected);
			reflect_prob = 1.0f;
		}
		if (drand48() < reflect_prob)
			srec.m_scatterRay = Ray(rec.m_position, reflected);
		else
			srec.m_scatterRay = Ray(rec.m_position, refracted);
		return true;
	}

	bool DiffuseLight::scatter(const Ray & in, const HitRecord & rec, ScatterRecord & srec) const
	{
		return false;
		srec.m_isSpecular = false;
		srec.m_attenuation = TextureMgr::getSingleton()->getTexture(m_albedo)
			->sample(rec.m_texcoord.x, rec.m_texcoord.y, rec.m_position);
		srec.m_pdf.reset(new CosinePDF(rec.m_normal));
		return true;
	}

	AVector3f DiffuseLight::emitted(const Ray &in, const HitRecord &rec, const Float & u,
		const Float & v, const AVector3f & p) const
	{
		return TextureMgr::getSingleton()->getTexture(m_emitTex)->sample(u, v, p);
	}
}
