#ifndef ARSPECTRUM_H
#define ARSPECTRUM_H

#include "ArAurora.h"

namespace Aurora
{
	template <int nSpectrumSamples>
	class ACoefficientSpectrum
	{
	public:
		ACoefficientSpectrum(Float v = 0.f)
		{
			for (int i = 0; i < nSpectrumSamples; ++i)
				c[i] = v;
		}

		ACoefficientSpectrum &operator+=(const ACoefficientSpectrum &s2)
		{
			for (int i = 0; i < nSpectrumSamples; ++i)
				c[i] += s2.c[i];
			return *this;
		}

		ACoefficientSpectrum operator+(const ACoefficientSpectrum &s2) const
		{
			ACoefficientSpectrum ret = *this;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] += s2.c[i];
			return ret;
		}

		ACoefficientSpectrum operator-(const ACoefficientSpectrum &s2) const
		{
			ACoefficientSpectrum ret = *this;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] -= s2.c[i];
			return ret;
		}

		ACoefficientSpectrum operator/(const ACoefficientSpectrum &s2) const
		{
			ACoefficientSpectrum ret = *this;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] /= s2.c[i];
			return ret;
		}

		ACoefficientSpectrum operator*(const ACoefficientSpectrum &sp) const
		{
			ACoefficientSpectrum ret = *this;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] *= sp.c[i];
			return ret;
		}

		ACoefficientSpectrum &operator*=(const ACoefficientSpectrum &sp)
		{
			for (int i = 0; i < nSpectrumSamples; ++i)
				c[i] *= sp.c[i];
			return *this;
		}

		ACoefficientSpectrum operator*(Float a) const
		{
			ACoefficientSpectrum ret = *this;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] *= a;
			return ret;
		}

		ACoefficientSpectrum &operator*=(Float a)
		{
			for (int i = 0; i < nSpectrumSamples; ++i)
				c[i] *= a;
			return *this;
		}

		friend inline ACoefficientSpectrum operator*(Float a, const ACoefficientSpectrum &s)
		{
			return s * a;
		}

		ACoefficientSpectrum operator/(Float a) const
		{
			ACoefficientSpectrum ret = *this;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] /= a;
			return ret;
		}

		ACoefficientSpectrum &operator/=(Float a)
		{
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
			return ret;
		}

		ACoefficientSpectrum clamp(Float low = 0, Float high = Infinity) const
		{
			ACoefficientSpectrum ret;
			for (int i = 0; i < nSpectrumSamples; ++i)
				ret.c[i] = Aurora::clamp(c[i], low, high);
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

		Float &operator[](int i) { return c[i]; }
		Float operator[](int i) const { return c[i]; }

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
			return s;
		}

		void toRGB(Float *rgb) const
		{
			rgb[0] = c[0];
			rgb[1] = c[1];
			rgb[2] = c[2];
		}

		const ARGBSpectrum &toRGBSpectrum() const { return *this; }

		Float y() const
		{
			const Float YWeight[3] = { 0.212671f, 0.715160f, 0.072169f };
			return YWeight[0] * c[0] + YWeight[1] * c[1] + YWeight[2] * c[2];
		}
	};
}

#endif