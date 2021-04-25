#include "ArSampler.h"

#include "ArCamera.h"

namespace Aurora
{
	//-------------------------------------------ASampler-------------------------------------

	ASampler::~ASampler() {}

	ASampler::ASampler(int64_t samplesPerPixel) : samplesPerPixel(samplesPerPixel) {}

	ACameraSample ASampler::getCameraSample(const AVector2i &pRaster)
	{
		ACameraSample cs;
		cs.pFilm = (AVector2f)pRaster + get2D();
		return cs;
	}

	void ASampler::startPixel(const AVector2i &p)
	{
		m_currentPixel = p;
		m_currentPixelSampleIndex = 0;
		// Reset array offsets for next pixel sample
		m_array1DOffset = m_array2DOffset = 0;
	}

	bool ASampler::startNextSample()
	{
		// Reset array offsets for next pixel sample
		m_array1DOffset = m_array2DOffset = 0;
		return ++m_currentPixelSampleIndex < samplesPerPixel;
	}

	bool ASampler::setSampleNumber(int64_t sampleNum)
	{
		// Reset array offsets for next pixel sample
		m_array1DOffset = m_array2DOffset = 0;
		m_currentPixelSampleIndex = sampleNum;
		return m_currentPixelSampleIndex < samplesPerPixel;
	}

	void ASampler::request1DArray(int n)
	{
		CHECK_EQ(roundCount(n), n);
		m_samples1DArraySizes.push_back(n);
		m_sampleArray1D.push_back(std::vector<Float>(n * samplesPerPixel));
	}

	void ASampler::request2DArray(int n)
	{
		CHECK_EQ(roundCount(n), n);
		m_samples2DArraySizes.push_back(n);
		m_sampleArray2D.push_back(std::vector<AVector2f>(n * samplesPerPixel));
	}

	const Float *ASampler::get1DArray(int n)
	{
		if (m_array1DOffset == m_sampleArray1D.size())
			return nullptr;
		CHECK_EQ(m_samples1DArraySizes[m_array1DOffset], n);
		CHECK_LT(m_currentPixelSampleIndex, samplesPerPixel);
		return &m_sampleArray1D[m_array1DOffset++][m_currentPixelSampleIndex * n];
	}

	const AVector2f *ASampler::get2DArray(int n)
	{
		if (m_array2DOffset == m_sampleArray2D.size())
			return nullptr;
		CHECK_EQ(m_samples2DArraySizes[m_array2DOffset], n);
		CHECK_LT(m_currentPixelSampleIndex, samplesPerPixel);
		return &m_sampleArray2D[m_array2DOffset++][m_currentPixelSampleIndex * n];
	}

	//-------------------------------------------ARandomSampler-------------------------------------

	ARandomSampler::ARandomSampler(int ns, int seed) : ASampler(ns), m_rng(seed) {}

	Float ARandomSampler::get1D()
	{
		CHECK_LT(m_currentPixelSampleIndex, samplesPerPixel);
		return m_rng.uniformFloat();
	}

	AVector2f ARandomSampler::get2D()
	{
		CHECK_LT(m_currentPixelSampleIndex, samplesPerPixel);
		return { m_rng.uniformFloat(), m_rng.uniformFloat() };
	}

	std::unique_ptr<ASampler> ARandomSampler::clone(int seed)
	{
		ARandomSampler *rs = new ARandomSampler(*this);
		rs->m_rng.setSequence(seed);
		return std::unique_ptr<ASampler>(rs);
	}

	void ARandomSampler::startPixel(const AVector2i &p)
	{
		for (size_t i = 0; i < m_sampleArray1D.size(); ++i)
			for (size_t j = 0; j < m_sampleArray1D[i].size(); ++j)
				m_sampleArray1D[i][j] = m_rng.uniformFloat();

		for (size_t i = 0; i < m_sampleArray2D.size(); ++i)
			for (size_t j = 0; j < m_sampleArray2D[i].size(); ++j)
				m_sampleArray2D[i][j] = { m_rng.uniformFloat(), m_rng.uniformFloat() };

		ASampler::startPixel(p);
	}

	//-------------------------------------------SamplingAlgorithm-------------------------------------

	AVector3f uniformSampleHemisphere(const AVector2f &u)
	{
		Float z = u[0];
		Float r = glm::sqrt(glm::max((Float)0, (Float)1. - z * z));
		Float phi = 2 * aPi * u[1];
		return AVector3f(r * glm::cos(phi), r * glm::sin(phi), z);
	}

	Float uniformHemispherePdf() { return aInv2Pi; }

	AVector3f uniformSampleSphere(const AVector2f &u)
	{
		Float z = 1 - 2 * u[0];
		Float r = glm::sqrt(glm::max((Float)0, (Float)1 - z * z));
		Float phi = 2 * aPi * u[1];
		return AVector3f(r * glm::cos(phi), r * glm::sin(phi), z);
	}

	Float uniformSpherePdf() { return aInv4Pi; }

	AVector3f uniformSampleCone(const AVector2f &u, Float cosThetaMax)
	{
		Float cosTheta = ((Float)1 - u[0]) + u[0] * cosThetaMax;
		Float sinTheta = glm::sqrt((Float)1 - cosTheta * cosTheta);
		Float phi = u[1] * 2 * aPi;
		return AVector3f(glm::cos(phi) * sinTheta, glm::sin(phi) * sinTheta, cosTheta);
	}

	AVector3f uniformSampleCone(const AVector2f &u, Float cosThetaMax, const AVector3f &x,
		const AVector3f &y, const AVector3f &z)
	{
		Float cosTheta = lerp(u[0], cosThetaMax, 1.f);
		Float sinTheta = glm::sqrt((Float)1. - cosTheta * cosTheta);
		Float phi = u[1] * 2 * aPi;
		return glm::cos(phi) * sinTheta * x + glm::sin(phi) * sinTheta * y + cosTheta * z;
	}

	Float uniformConePdf(Float cosThetaMax) { return 1 / (2 * aPi * (1 - cosThetaMax)); }

	AVector2f concentricSampleDisk(const AVector2f &u)
	{
		// Map uniform random numbers to $[-1,1]^2$
		AVector2f uOffset = 2.f * u - AVector2f(1, 1);

		// Handle degeneracy at the origin
		if (uOffset.x == 0 && uOffset.y == 0)
			return AVector2f(0, 0);

		// Apply concentric mapping to point
		Float theta, r;
		if (glm::abs(uOffset.x) > glm::abs(uOffset.y))
		{
			r = uOffset.x;
			theta = aPiOver4 * (uOffset.y / uOffset.x);
		}
		else
		{
			r = uOffset.y;
			theta = aPiOver2 - aPiOver4 * (uOffset.x / uOffset.y);
		}
		return r * AVector2f(glm::cos(theta), glm::sin(theta));
	}

	AVector2f uniformSampleTriangle(const AVector2f &u)
	{
		Float su0 = glm::sqrt(u[0]);
		return AVector2f(1 - su0, u[1] * su0);
	}

}