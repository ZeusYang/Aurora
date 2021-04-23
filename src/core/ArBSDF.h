#ifndef ARBSDF_H
#define ARBSDF_H

#include "ArAurora.h"
#include "ArInteraction.h"
#include "ArMathUtils.h"
#include "ArSpectrum.h"

namespace Aurora
{
	enum ABxDFType
	{
		BSDF_REFLECTION = 1 << 0,
		BSDF_TRANSMISSION = 1 << 1,
		BSDF_DIFFUSE = 1 << 2,
		BSDF_GLOSSY = 1 << 3,
		BSDF_SPECULAR = 1 << 4,
		BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION,
	};

	Float frDielectric(Float cosThetaI, Float etaI, Float etaT);

	inline bool sameHemisphere(const AVector3f &w, const AVector3f &wp) { return w.z * wp.z > 0; }

	//inline AVector3f reflect(const AVector3f &wo, const AVector3f &n) { return -wo + 2 * dot(wo, n) * n; }

	//inline bool refract(const AVector3f &wi, const AVector3f &n, Float eta, AVector3f &wt)
	//{
	//	// Compute $\cos \theta_\roman{t}$ using Snell's law
	//	Float cosThetaI = dot(n, wi);
	//	Float sin2ThetaI = glm::max(Float(0), Float(1 - cosThetaI * cosThetaI));
	//	Float sin2ThetaT = eta * eta * sin2ThetaI;

	//	// Handle total internal reflection for transmission
	//	if (sin2ThetaT >= 1) return false;
	//	Float cosThetaT = std::sqrt(1 - sin2ThetaT);
	//	wt = eta * -wi + (eta * cosThetaI - cosThetaT) * AVector3f(n);
	//	return true;
	//}

	class ABSDF
	{
	public:
		typedef std::shared_ptr<ABSDF> ptr;

		// BSDF Public Methods
		ABSDF(const ASurfaceInteraction &si, Float eta = 1)
			: m_eta(eta), m_ns(si.n), m_ss(normalize(si.dpdu)), m_ts(cross(m_ns, m_ss)) {}

		~ABSDF() = default;

		void add(ABxDF *b) { m_bxdfs[m_nBxDFs++] = b; }

		int numComponents(ABxDFType flags = BSDF_ALL) const;

		AVector3f worldToLocal(const AVector3f &v) const
		{
			return AVector3f(dot(v, m_ss), dot(v, m_ts), dot(v, m_ns));
		}

		AVector3f localToWorld(const AVector3f &v) const
		{
			return AVector3f(
				m_ss.x * v.x + m_ts.x * v.y + m_ns.x * v.z,
				m_ss.y * v.x + m_ts.y * v.y + m_ns.y * v.z,
				m_ss.z * v.x + m_ts.z * v.y + m_ns.z * v.z);
		}

		ASpectrum f(const AVector3f &woW, const AVector3f &wiW, ABxDFType flags = BSDF_ALL) const;

		ASpectrum sample_f(const AVector3f &wo, AVector3f &wi, const AVector2f &u, Float &pdf,
			ABxDFType &sampledType, ABxDFType type = BSDF_ALL) const;

		Float pdf(const AVector3f &wo, const AVector3f &wi, ABxDFType flags = BSDF_ALL) const;

		//Refractive index
		const Float m_eta;

	private:
		int m_nBxDFs = 0;
		const AVector3f m_ns, m_ss, m_ts;

		static constexpr int NumMaxBxDFs = 8;
		ABxDF *m_bxdfs[NumMaxBxDFs];
	};

	class ABxDF
	{
	public:
		ABxDF(ABxDFType type) : m_type(type) {}

		virtual ~ABxDF() = default;

		bool matchesFlags(ABxDFType t) const { return (m_type & t) == m_type; }

		virtual ASpectrum f(const AVector3f &wo, const AVector3f &wi) const = 0;
		virtual ASpectrum sample_f(const AVector3f &wo, AVector3f &wi, const AVector2f &sample,
			Float &pdf, ABxDFType &sampledType) const;

		virtual Float pdf(const AVector3f &wo, const AVector3f &wi) const;

		// BxDF Public Data
		const ABxDFType m_type;
	};

	class AFresnel
	{
	public:
		virtual ~AFresnel() = default;
		virtual ASpectrum evaluate(Float cosI) const = 0;
	};

	class AFresnelDielectric : public AFresnel
	{
	public:
		AFresnelDielectric(Float etaI, Float etaT) : m_etaI(etaI), m_etaT(etaT) {}

		virtual ASpectrum evaluate(Float cosThetaI) const override
		{
			return frDielectric(cosThetaI, m_etaI, m_etaT);
		}

	private:
		Float m_etaI, m_etaT;
	};

	class AFresnelNoOp : public AFresnel
	{
	public:
		virtual ASpectrum evaluate(Float) const override { return ASpectrum(1.); }
	};

	class ALambertianReflection : public ABxDF
	{
	public:
		// LambertianReflection Public Methods
		ALambertianReflection(const ASpectrum &R)
			: ABxDF(ABxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), m_R(R) {}

		virtual ASpectrum f(const AVector3f &wo, const AVector3f &wi) const override;

	private:
		// LambertianReflection Private Data
		const ASpectrum m_R;
	};

	class ASpecularReflection : public ABxDF
	{
	public:
		// SpecularReflection Public Methods
		ASpecularReflection(const ASpectrum &R, AFresnel *fresnel)
			: ABxDF(ABxDFType(BSDF_REFLECTION | BSDF_SPECULAR)),
			m_R(R),
			m_fresnel(fresnel) {}

		virtual ASpectrum f(const AVector3f &wo, const AVector3f &wi) const override { return ASpectrum(0.f); }

		virtual ASpectrum sample_f(const AVector3f &wo, AVector3f &wi, const AVector2f &sample,
			Float &pdf, ABxDFType &sampledType) const override;

		virtual Float pdf(const AVector3f &wo, const AVector3f &wi) const override { return 0.f; }

	private:
		// SpecularReflection Private Data
		const ASpectrum m_R;
		const AFresnel *m_fresnel;
	};

	class ASpecularTransmission : public ABxDF
	{
	public:
		// SpecularTransmission Public Methods
		ASpecularTransmission(const ASpectrum &T, Float etaA, Float etaB, ATransportMode mode)
			: ABxDF(ABxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)), m_T(T), m_etaA(etaA),
			m_etaB(etaB), m_fresnel(etaA, etaB), m_mode(mode) {}

		virtual ASpectrum f(const AVector3f &wo, const AVector3f &wi) const override { return ASpectrum(0.f); }

		virtual ASpectrum sample_f(const AVector3f &wo, AVector3f &wi, const AVector2f &sample,
			Float &pdf, ABxDFType &sampledType) const override;

		virtual Float pdf(const AVector3f &wo, const AVector3f &wi) const override { return 0.f; }

	private:
		const ASpectrum m_T;
		const Float m_etaA, m_etaB;
		const AFresnelDielectric m_fresnel;
		const ATransportMode m_mode;
	};

}

#endif 