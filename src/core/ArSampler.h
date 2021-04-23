#ifndef ARSAMPLER_H
#define ARSAMPLER_H

#include "ArAurora.h"
#include "ArMathUtils.h"

#include "ArRng.h"

#include <vector>

namespace Aurora
{
	class ASampler
	{
	public:
		typedef std::shared_ptr<ASampler> ptr;

		virtual ~ASampler();
		ASampler(int64_t samplesPerPixel);

		virtual void startPixel(const AVector2i &p);
		virtual Float get1D() = 0;
		virtual AVector2f get2D() = 0;
		ACameraSample getCameraSample(const AVector2i &pRaster);

		void request1DArray(int n);
		void request2DArray(int n);

		virtual int roundCount(int n) const { return n; }

		const Float *get1DArray(int n);
		const AVector2f *get2DArray(int n)
			;
		virtual bool startNextSample();

		virtual std::unique_ptr<ASampler> Clone(int seed) = 0;
		virtual bool setSampleNumber(int64_t sampleNum);

		int64_t currentSampleNumber() const { return m_currentPixelSampleIndex; }

		int64_t getSamplingNumber() const { return samplesPerPixel; }

		const int64_t samplesPerPixel; //Number of sampling per pixel

	protected:
		AVector2i m_currentPixel;
		int64_t m_currentPixelSampleIndex;
		std::vector<int> m_samples1DArraySizes, m_samples2DArraySizes;
		std::vector<std::vector<Float>> m_sampleArray1D;
		std::vector<std::vector<AVector2f>> m_sampleArray2D;

	private:
		size_t m_array1DOffset, m_array2DOffset;
	};

	class ARandomSampler final : public ASampler
	{
	public:
		typedef std::shared_ptr<ARandomSampler> ptr;

		ARandomSampler(int ns, int seed = 0);

		virtual void startPixel(const AVector2i &) override;

		virtual Float get1D() override;
		virtual AVector2f get2D() override;

		virtual std::unique_ptr<ASampler> Clone(int seed) override;

	private:
		ARng m_rng; //Random number generator
	};

	AVector3f uniformSampleHemisphere(const AVector2f &u);
	Float uniformHemispherePdf();
	AVector3f uniformSampleSphere(const AVector2f &u);
	Float uniformSpherePdf();

	AVector3f uniformSampleCone(const AVector2f &u, Float thetamax);
	AVector3f uniformSampleCone(const AVector2f &u, Float thetamax, const AVector3f &x,
		const AVector3f &y, const AVector3f &z);
	Float uniformConePdf(Float thetamax);

	AVector2f concentricSampleDisk(const AVector2f &u);

	AVector2f uniformSampleTriangle(const AVector2f &u);

	inline AVector3f cosineSampleHemisphere(const AVector2f &u)
	{
		AVector2f d = concentricSampleDisk(u);
		Float z = std::sqrt(glm::max((Float)0, 1 - d.x * d.x - d.y * d.y));
		return AVector3f(d.x, d.y, z);
	}

	inline Float cosineHemispherePdf(Float cosTheta) { return cosTheta * aInvPi; }

	inline Float balanceHeuristic(int nf, Float fPdf, int ng, Float gPdf)
	{
		return (nf * fPdf) / (nf * fPdf + ng * gPdf);
	}

	inline Float powerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
	{
		Float f = nf * fPdf, g = ng * gPdf;
		return (f * f) / (f * f + g * g);
	}
}

#endif