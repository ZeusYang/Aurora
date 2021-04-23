#include "ArBSDF.h"

#include "ArSampler.h"

namespace Aurora
{
	//-------------------------------------------ABSDF-------------------------------------

	int ABSDF::numComponents(ABxDFType flags) const
	{
		int num = 0;
		for (int i = 0; i < m_nBxDFs; ++i)
			if (m_bxdfs[i]->matchesFlags(flags))
				++num;
		return num;
	}

	ASpectrum ABSDF::f(const AVector3f &woW, const AVector3f &wiW, ABxDFType flags) const
	{
		AVector3f wi = worldToLocal(wiW), wo = worldToLocal(woW);
		if (wo.z == 0)
			return 0.f;

		bool reflect = dot(wiW, m_ns) * dot(woW, m_ns) > 0;
		ASpectrum f(0.f);
		for (int i = 0; i < m_nBxDFs; ++i)
		{
			if (m_bxdfs[i]->matchesFlags(flags) &&
				((reflect && (m_bxdfs[i]->m_type & BSDF_REFLECTION)) ||
				(!reflect && (m_bxdfs[i]->m_type & BSDF_TRANSMISSION))))
			{
				f += m_bxdfs[i]->f(wo, wi);
			}
		}
		return f;
	}

	ASpectrum ABSDF::sample_f(const AVector3f &wo, AVector3f &wi, const AVector2f &u, Float &pdf,
		ABxDFType &sampledType, ABxDFType type) const
	{
		return ASpectrum(1.0f);
	}

	Float ABSDF::pdf(const AVector3f &woWorld, const AVector3f &wiWorld, ABxDFType flags) const
	{
		if (m_nBxDFs == 0)
			return 0.f;

		AVector3f wo = worldToLocal(woWorld), wi = worldToLocal(wiWorld);

		if (wo.z == 0)
			return 0.;

		Float pdf = 0.f;
		int matchingComps = 0;
		for (int i = 0; i < m_nBxDFs; ++i) {
			if (m_bxdfs[i]->matchesFlags(flags))
			{
				++matchingComps;
				pdf += m_bxdfs[i]->pdf(wo, wi);
			}
		}
		Float v = matchingComps > 0 ? pdf / matchingComps : 0.f;
		return v;
	}

	//-------------------------------------------ABxDF-------------------------------------

	ASpectrum ABxDF::sample_f(const AVector3f &wo, AVector3f &wi, const AVector2f &sample,
		Float &pdf, ABxDFType &sampledType) const
	{
		// Cosine-sample the hemisphere, flipping the direction if necessary
		wi = cosineSampleHemisphere(sample);
		if (wo.z < 0)
			wi.z *= -1;

		pdf = this->pdf(wo, wi);

		return f(wo, wi);
	}

	Float ABxDF::pdf(const AVector3f &wo, const AVector3f &wi) const
	{
		return sameHemisphere(wo, wi) ? glm::abs(wi.z) * aInvPi : 0;
	}

	//-------------------------------------------ALambertianReflection-------------------------------------

	ASpectrum ALambertianReflection::f(const AVector3f &wo, const AVector3f &wi) const
	{
		return m_R * aInvPi;
	}

	//-------------------------------------------ASpecularReflection-------------------------------------

	ASpectrum ASpecularReflection::sample_f(const AVector3f &wo, AVector3f &wi, const AVector2f &sample,
		Float &pdf, ABxDFType &sampledType) const
	{
		wi = AVector3f(-wo.x, -wo.y, wo.z);
		pdf = 1;
		return m_fresnel->evaluate(wi.z) * m_R / glm::abs(wi.z);
	}

	//-------------------------------------------ASpecularTransmission-------------------------------------

	ASpectrum ASpecularTransmission::sample_f(const AVector3f &wo, AVector3f &wi, const AVector2f &sample,
		Float &pdf, ABxDFType &sampledType) const
	{
		// Figure out which $\eta$ is incident and which is transmitted
		bool entering = (wo.z) > 0;
		Float etaI = entering ? m_etaA : m_etaB;
		Float etaT = entering ? m_etaB : m_etaA;

		// Compute ray direction for specular transmission
		if (!refract(wo, faceforward(AVector3f(0, 0, 1), wo), etaI / etaT, wi))
			return 0;

		pdf = 1;
		ASpectrum ft = m_T * (ASpectrum(1.) - m_fresnel.evaluate(wi.z));

		// Account for non-symmetry with transmission to different medium
		if (m_mode == ATransportMode::aRadiance)
			ft *= (etaI * etaI) / (etaT * etaT);
		return ft / glm::abs(wi.z);
	}

	//-------------------------------------------Utility function-------------------------------------

	Float frDielectric(Float cosThetaI, Float etaI, Float etaT)
	{
		cosThetaI = clamp(cosThetaI, -1, 1);
		// Potentially swap indices of refraction
		bool entering = cosThetaI > 0.f;
		if (!entering)
		{
			std::swap(etaI, etaT);
			cosThetaI = glm::abs(cosThetaI);
		}

		// Compute _cosThetaT_ using Snell's law
		Float sinThetaI = glm::sqrt(glm::max((Float)0, 1 - cosThetaI * cosThetaI));
		Float sinThetaT = etaI / etaT * sinThetaI;

		// Handle total internal reflection
		if (sinThetaT >= 1)
			return 1;
		Float cosThetaT = glm::sqrt(glm::max((Float)0, 1 - sinThetaT * sinThetaT));
		Float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
		Float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));
		return (Rparl * Rparl + Rperp * Rperp) / 2;
	}
}