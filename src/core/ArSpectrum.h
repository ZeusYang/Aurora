#ifndef ARSPECTRUM_H
#define ARSPECTRUM_H

#include "ArAurora.h"

namespace Aurora
{

	inline void XYZToRGB(const Float xyz[3], Float rgb[3]) 
	{
		rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
		rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
		rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
	}

	inline void RGBToXYZ(const Float rgb[3], Float xyz[3]) 
	{
		xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
		xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
		xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];
	}

	enum class ASpectrumType { Reflectance, Illuminant };

	template <int nSpectrumSamples>
	class ACoefficientSpectrum
	{
	public:
		ACoefficientSpectrum(Float v = 0.f)
		{
			for (int i = 0; i < nSpectrumSamples; ++i)
				c[i] = v;
			DCHECK(!hasNaNs());
		}

		ACoefficientSpectrum &operator+=(const ACoefficientSpectrum &s2)
		{
			DCHECK(!s2.hasNaNs());
			for (int i = 0; i < nSpectrumSamples; ++i)
				c[i] += s2.c[i];
			return *this;
		}

		ACoefficientSpectrum operator+(const ACoefficientSpectrum &s2) const
		{
			DCHECK(!s2.hasNaNs());
			ACoefficientSpectrum ret = *this;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] += s2.c[i];
			return ret;
		}

		ACoefficientSpectrum operator-(const ACoefficientSpectrum &s2) const
		{
			DCHECK(!s2.hasNaNs());
			ACoefficientSpectrum ret = *this;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] -= s2.c[i];
			return ret;
		}

		ACoefficientSpectrum operator/(const ACoefficientSpectrum &s2) const
		{
			DCHECK(!s2.hasNaNs());
			ACoefficientSpectrum ret = *this;
			for (int i = 0; i < nSpectrumSamples; ++i)
			{
				CHECK_NE(s2.c[i], 0);
				ret.c[i] /= s2.c[i];
			}
			return ret;
		}

		ACoefficientSpectrum operator*(const ACoefficientSpectrum &sp) const
		{
			DCHECK(!sp.hasNaNs());
			ACoefficientSpectrum ret = *this;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] *= sp.c[i];
			return ret;
		}

		ACoefficientSpectrum &operator*=(const ACoefficientSpectrum &sp)
		{
			DCHECK(!sp.hasNaNs());
			for (int i = 0; i < nSpectrumSamples; ++i)
				c[i] *= sp.c[i];
			return *this;
		}

		ACoefficientSpectrum operator*(Float a) const
		{
			ACoefficientSpectrum ret = *this;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] *= a;
			DCHECK(!ret.hasNaNs());
			return ret;
		}

		ACoefficientSpectrum &operator*=(Float a)
		{
			for (int i = 0; i < nSpectrumSamples; ++i)
				c[i] *= a;
			DCHECK(!hasNaNs());
			return *this;
		}

		friend inline ACoefficientSpectrum operator*(Float a, const ACoefficientSpectrum &s)
		{
			DCHECK(!glm::isnan(a) && !s.hasNaNs());
			return s * a;
		}

		ACoefficientSpectrum operator/(Float a) const
		{
			CHECK_NE(a, 0);
			DCHECK(!glm::isnan(a));
			ACoefficientSpectrum ret = *this;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] /= a;
			DCHECK(!ret.hasNaNs());
			return ret;
		}

		ACoefficientSpectrum &operator/=(Float a)
		{
			CHECK_NE(a, 0);
			DCHECK(!glm::isnan(a));
			for (int i = 0; i < nSpectrumSamples; ++i)
				c[i] /= a;
			return *this;
		}

		bool operator==(const ACoefficientSpectrum &sp) const
		{
			for (int i = 0; i < nSpectrumSamples; ++i)
			{
				if (c[i] != sp.c[i])
					return false;
			}
			return true;
		}

		bool operator!=(const ACoefficientSpectrum &sp) const
		{
			return !(*this == sp);
		}

		bool isBlack() const
		{
			for (int i = 0; i < nSpectrumSamples; ++i)
			{
				if (c[i] != 0.)
					return false;
			}
			return true;
		}

		friend ACoefficientSpectrum sqrt(const ACoefficientSpectrum &s)
		{
			ACoefficientSpectrum ret;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] = glm::sqrt(s.c[i]);
			DCHECK(!ret.hasNaNs());
			return ret;
		}

		template <int n>
		friend inline ACoefficientSpectrum<n> pow(const ACoefficientSpectrum<n> &s, Float e);

		ACoefficientSpectrum operator-() const
		{
			ACoefficientSpectrum ret;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] = -c[i];
			return ret;
		}

		friend ACoefficientSpectrum exp(const ACoefficientSpectrum &s)
		{
			ACoefficientSpectrum ret;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] = glm::exp(s.c[i]);
			DCHECK(!ret.hasNaNs());
			return ret;
		}

		friend std::ostream &operator<<(std::ostream &os, const ACoefficientSpectrum &s) 
		{
			return os << s.toString();
		}

		std::string toString() const 
		{
			std::string str = "[ ";
			for (int i = 0; i < nSpectrumSamples; ++i) 
			{
				str += stringPrintf("%f", c[i]);
				if (i + 1 < nSpectrumSamples) 
					str += ", ";
			}
			str += " ]";
			return str;
		}

		ACoefficientSpectrum clamp(Float low = 0, Float high = aInfinity) const
		{
			ACoefficientSpectrum ret;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] = Aurora::clamp(c[i], low, high);
			DCHECK(!ret.hasNaNs());
			return ret;
		}

		Float maxComponentValue() const
		{
			Float m = c[0];
			for (int i = 1; i < nSpectrumSamples; ++i)
				m = glm::max(m, c[i]);
			return m;
		}

		bool hasNaNs() const
		{
			for (int i = 0; i < nSpectrumSamples; ++i)
			{
				if (glm::isnan(c[i]))
					return true;
			}
			return false;
		}

		Float &operator[](int i) 
		{
			DCHECK(i >= 0 && i < nSpectrumSamples);
			return c[i]; 
		}

		Float operator[](int i) const 
		{ 
			DCHECK(i >= 0 && i < nSpectrumSamples);
			return c[i]; 
		}

		static const int nSamples = nSpectrumSamples;

	protected:
		Float c[nSpectrumSamples];
	};

	class ARGBSpectrum : public ACoefficientSpectrum<3>
	{
		using ACoefficientSpectrum<3>::c;

	public:
		ARGBSpectrum(Float v = 0.f) : ACoefficientSpectrum<3>(v) {}
		ARGBSpectrum(const ACoefficientSpectrum<3> &v) : ACoefficientSpectrum<3>(v) {}
		ARGBSpectrum(const ARGBSpectrum &s) { *this = s; }

		static ARGBSpectrum fromRGB(const Float rgb[3])
		{
			ARGBSpectrum s;
			s.c[0] = rgb[0];
			s.c[1] = rgb[1];
			s.c[2] = rgb[2];
			DCHECK(!s.hasNaNs());
			return s;
		}

		void toRGB(Float *rgb) const
		{
			rgb[0] = c[0];
			rgb[1] = c[1];
			rgb[2] = c[2];
		}

		void toXYZ(Float xyz[3]) const { RGBToXYZ(c, xyz); }
		const ARGBSpectrum &toRGBSpectrum() const { return *this; }
		
		static ARGBSpectrum fromXYZ(const Float xyz[3], ASpectrumType type = ASpectrumType::Reflectance) 
		{
			ARGBSpectrum r;
			XYZToRGB(xyz, r.c);
			return r;
		}

		Float y() const
		{
			const Float YWeight[3] = { 0.212671f, 0.715160f, 0.072169f };
			return YWeight[0] * c[0] + YWeight[1] * c[1] + YWeight[2] * c[2];
		}
	};

	// Spectrum Inline Functions
	template <int nSpectrumSamples>
	inline ACoefficientSpectrum<nSpectrumSamples> pow(
		const ACoefficientSpectrum<nSpectrumSamples> &s, Float e) 
	{
		ACoefficientSpectrum<nSpectrumSamples> ret;
		for (int i = 0; i < nSpectrumSamples; ++i)
		{
			ret.c[i] = glm::pow(s.c[i], e);
		}
		DCHECK(!ret.hasNaNs());
		return ret;
	}
}

#endif