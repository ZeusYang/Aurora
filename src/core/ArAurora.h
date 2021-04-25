#ifndef ARAURORA_H
#define ARAURORA_H

#include <limits>
#include <memory>

#include "glog/logging.h"

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

	//-------------------------------------------stringPrintf-------------------------------------

	inline void stringPrintfRecursive(std::string *s, const char *fmt) 
	{
		const char *c = fmt;
		// No args left; make sure there aren't any extra formatting
		// specifiers.
		while (*c) 
		{
			if (*c == '%') 
			{
				CHECK_EQ(c[1], '%');
				++c;
			}
			*s += *c++;
		}
	}

	// 1. Copy from fmt to *s, up to the next formatting directive.
	// 2. Advance fmt past the next formatting directive and return the
	//    formatting directive as a string.
	inline std::string copyToFormatString(const char **fmt_ptr, std::string *s) 
	{
		const char *&fmt = *fmt_ptr;
		while (*fmt) 
		{
			if (*fmt != '%') 
			{
				*s += *fmt;
				++fmt;
			}
			else if (fmt[1] == '%') 
			{
				// "%%"; let it pass through
				*s += '%';
				*s += '%';
				fmt += 2;
			}
			else
				// fmt is at the start of a formatting directive.
				break;
		}

		std::string nextFmt;
		if (*fmt) 
		{
			do 
			{
				nextFmt += *fmt;
				++fmt;
				// Incomplete (but good enough?) test for the end of the
				// formatting directive: a new formatting directive starts, we
				// hit whitespace, or we hit a comma.
			} while (*fmt && *fmt != '%' && !isspace(*fmt) && *fmt != ',' &&
				*fmt != '[' && *fmt != ']' && *fmt != '(' && *fmt != ')');
		}

		return nextFmt;
	}

	template <typename T>
	inline std::string formatOne(const char *fmt, T v) 
	{
		// Figure out how much space we need to allocate; add an extra
		// character for the '\0'.
		size_t size = snprintf(nullptr, 0, fmt, v) + 1;
		std::string str;
		str.resize(size);
		snprintf(&str[0], size, fmt, v);
		str.pop_back();  // remove trailing NUL
		return str;
	}

	// General-purpose version of stringPrintfRecursive; add the formatted
	// output for a single StringPrintf() argument to the final result string
	// in *s.
	template <typename T, typename... Args>
	inline void stringPrintfRecursive(std::string *s, const char *fmt, T v, Args... args) 
	{
		std::string nextFmt = copyToFormatString(&fmt, s);
		*s += formatOne(nextFmt.c_str(), v);
		stringPrintfRecursive(s, fmt, args...);
	}

	// Special case of StringPrintRecursive for float-valued arguments.
	template <typename... Args>
	inline void stringPrintfRecursive(std::string *s, const char *fmt, float v, Args... args) 
	{
		std::string nextFmt = copyToFormatString(&fmt, s);
		if (nextFmt == "%f")
			// Always use enough precision so that the printed value gives
			// the exact floating-point value if it's used to initialize a
			// float.
			// https://randomascii.wordpress.com/2012/03/08/float-precisionfrom-zero-to-100-digits-2/
			*s += formatOne("%.9g", v);
		else
			// If a specific formatting string other than "%f" was specified,
			// just use that.
			*s += formatOne(nextFmt.c_str(), v);

		// Go forth and print the next arg.
		stringPrintfRecursive(s, fmt, args...);
	}

	// Specialization for doubles that always uses enough precision.  (It seems
	// that this is the version that is actually called for floats.  I thought
	// that float->double promotion wasn't supposed to happen in this case?)
	template <typename... Args>
	inline void stringPrintfRecursive(std::string *s, const char *fmt, double v, Args... args) 
	{
		std::string nextFmt = copyToFormatString(&fmt, s);
		if (nextFmt == "%f")
			*s += formatOne("%.17g", v);
		else
			*s += formatOne(nextFmt.c_str(), v);
		stringPrintfRecursive(s, fmt, args...);
	}

	// StringPrintf() is a replacement for sprintf() (and the like) that
	// returns the result as a std::string. This gives convenience/control
	// of printf-style formatting in a more C++-ish way.
	//
	// Floating-point values with the formatting string "%f" are handled
	// specially so that enough digits are always printed so that the original
	// float/double can be reconstituted exactly from the printed digits.
	template <typename... Args>
	inline std::string stringPrintf(const char *fmt, Args... args) 
	{
		std::string ret;
		stringPrintfRecursive(&ret, fmt, args...);
		return ret;
	}

}

#endif