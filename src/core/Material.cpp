#include "Material.h"

#include "TextureMgr.h"
#include "ONB.h"
#include "CosinePDF.h"

namespace RayTracer
{
	bool Lambertian::scatter(const Ray &in, const HitRecord &rec, ScatterRecord &srec) const
	{
		srec.m_isSpecular = false;
		srec.m_attenuation = TextureMgr::getSingleton()->getTexture(m_albedo)
			->sample(rec.m_texcoord.x, rec.m_texcoord.y, rec.m_position);
		srec.m_pdf.reset(new CosinePDF(rec.m_normal));
		return true;
	}

	float Lambertian::scattering_pdf(const Ray &in, const HitRecord &rec, const Ray &scattered) const
	{
		float cosine = rec.m_normal.dotProduct(scattered.getDirection());
		if (cosine < 0.0f)
			cosine = 0.0f;
		return cosine / M_PI;
	}

	bool Metal::scatter(const Ray &in, const HitRecord &rec, ScatterRecord &srec) const
	{
		Vector3D reflectedDir = Vector3D::reflect(in.getDirection(), rec.m_normal);
		srec.m_scatterRay = Ray(rec.m_position, reflectedDir + Vector3D::randomInUnitSphere() * m_fuzz);
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
		Vector3D outward_normal;
		Vector3D reflected = Vector3D::reflect(in.getDirection(), rec.m_normal);
		float ni_over_nt;
		srec.m_attenuation = Vector3D(1.0f, 1.0f, 1.0f);
		Vector3D refracted;
		float reflect_prob;
		float cosine;
		// from inside to outside.
		if (in.getDirection().dotProduct(rec.m_normal) > 0.0f)
		{
			outward_normal = -rec.m_normal;
			ni_over_nt = refIdx;
			cosine = /*refIdx * */in.getDirection().dotProduct(rec.m_normal) / in.getDirection().getLength();
		}
		// from outside to inside.
		else
		{
			outward_normal = rec.m_normal;
			ni_over_nt = 1.0 / refIdx;
			cosine = -in.getDirection().dotProduct(rec.m_normal) / in.getDirection().getLength();
		}

		if (Vector3D::refract(in.getDirection(), outward_normal, ni_over_nt, refracted))
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

	Vector3D DiffuseLight::emitted(const Ray &in, const HitRecord &rec, const float & u,
		const float & v, const Vector3D & p) const
	{
		return TextureMgr::getSingleton()->getTexture(m_emitTex)->sample(u, v, p);
	}
}
