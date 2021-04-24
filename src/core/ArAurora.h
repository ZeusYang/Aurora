#ifndef ARAURORA_H
#define ARAURORA_H

#include <limits>
#include <memory>

#if defined(_MSC_VER)
#define NOMINMAX
#endif

#define GLM_FORCE_LEFT_HANDED
#include "glm/glm.hpp"

// Platform-specific definitions
#if defined(_WIN32) || defined(_WIN64)
#define AURORA_WINDOWS_OS
#endif

#define ALLOCA(TYPE, COUNT) (TYPE *) alloca((COUNT) * sizeof(TYPE))

namespace Aurora
{

	//Float type
#ifdef AURORA_DOUBLE_AS_FLOAT
	typedef double Float;
#else
	typedef float Float;
#endif

	using Byte = unsigned char;

	constexpr static Float aShadowEpsilon = 0.0001f;
	constexpr static Float aPi = 3.14159265358979323846f;
	constexpr static Float aInvPi = 0.31830988618379067154f;
	constexpr static Float aInv2Pi = 0.15915494309189533577f;
	constexpr static Float aInv4Pi = 0.07957747154594766788f;
	constexpr static Float aPiOver2 = 1.57079632679489661923f;
	constexpr static Float aPiOver4 = 0.78539816339744830961f;
	constexpr static Float aSqrt2 = 1.41421356237309504880f;

	constexpr static Float aMaxFloat = std::numeric_limits<Float>::max();
	constexpr static Float aInfinity = std::numeric_limits<Float>::infinity();
	constexpr static Float aMachineEpsilon = std::numeric_limits<Float>::epsilon() * 0.5f;

	inline Float lerp(Float t, Float v1, Float v2) { return (1 - t) * v1 + t * v2; }
	inline Float gamma(int n) { return (n * aMachineEpsilon) / (1 - n * aMachineEpsilon); }

	inline Float gammaCorrect(Float value) 
	{
		if (value <= 0.0031308f)
			return 12.92f * value;
		return 1.055f * glm::pow(value, (Float)(1.f / 2.4f)) - 0.055f;
	}

	inline Float inverseGammaCorrect(Float value) 
	{
		if (value <= 0.04045f) 
			return value * 1.f / 12.92f;
		return glm::pow((value + 0.055f) * 1.f / 1.055f, (Float)2.4f);
	}

	template <typename T, typename U, typename V>
	inline T clamp(T val, U low, V high)
	{
		if (val < low)
			return low;
		else if (val > high)
			return high;
		else
			return val;
	}

	class ARay;
	class AFilm;
	class ABSDF;
	class ABxDF;
	class ALight;
	class AShape;
	class AScene;
	class ACamera;
	class AHitable;
	class AFilmTile;
	class ASampler;
	class AMaterial;
	class AAreaLight;
	class ATransform;
	class AIntegrator;
	class ACameraSample;
	class ARGBSpectrum;
	class AInteraction;
	class ADistribution1D;
	class AVisibilityTester;
	class ASurfaceInteraction;

	class MemoryArena;

	using ASpectrum = ARGBSpectrum;

	// TransportMode Declarations
	enum class ATransportMode { aRadiance, aImportance };

	inline uint32_t floatToBits(float f) 
	{
		uint32_t ui;
		memcpy(&ui, &f, sizeof(float));
		return ui;
	}

	inline float bitsToFloat(uint32_t ui) 
	{
		float f;
		memcpy(&f, &ui, sizeof(uint32_t));
		return f;
	}

	inline uint64_t floatToBits(double f) 
	{
		uint64_t ui;
		memcpy(&ui, &f, sizeof(double));
		return ui;
	}

	inline double bitsToFloat(uint64_t ui) 
	{
		double f;
		memcpy(&f, &ui, sizeof(uint64_t));
		return f;
	}

}

#endif