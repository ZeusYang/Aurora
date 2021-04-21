#ifndef ARAURORA_H
#define ARAURORA_H

#include <limits>
#include <memory>

#include "glm/glm.hpp"

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
	class ASampler;
	class AMaterial;
	class AAreaLight;
	class ATransform;
	class AIntegrator;
	class ACameraSample;
	class ARGBSpectrum;
	class AInteraction;
	class AVisibilityTester;
	class ASurfaceInteraction;

	class MemoryArena;

	using ASpectrum = ARGBSpectrum;

	// TransportMode Declarations
	enum class ATransportMode { aRadiance, aImportance };



}

#endif