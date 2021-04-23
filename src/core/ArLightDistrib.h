#ifndef ARLIGHT_DISTRIB_H
#define ARLIGHT_DISTRIB_H

#include "ArAurora.h"
#include "ArMathUtils.h"

#include <vector>

namespace Aurora
{
	class ADistribution1D 
	{
	public:

		ADistribution1D(const Float *f, int n) : func(f, f + n), cdf(n + 1) 
		{
			// Compute integral of step function at $x_i$
			cdf[0] = 0;
			for (int i = 1; i < n + 1; ++i) 
				cdf[i] = cdf[i - 1] + func[i - 1] / n;

			// Transform step function integral into CDF
			funcInt = cdf[n];
			if (funcInt == 0) 
			{
				for (int i = 1; i < n + 1; ++i) 
					cdf[i] = Float(i) / Float(n);
			}
			else 
			{
				for (int i = 1; i < n + 1; ++i) 
					cdf[i] /= funcInt;
			}
		}

		int count() const 
		{ 
			return (int)func.size();
		}

		Float sampleContinuous(Float u, Float *pdf, int *off = nullptr) const 
		{
			// Find surrounding CDF segments and _offset_
			int offset = findInterval((int)cdf.size(), [&](int index) 
			{ 
				return cdf[index] <= u; 
			});

			if (off) 
				*off = offset;

			// Compute offset along CDF segment
			Float du = u - cdf[offset];
			if ((cdf[offset + 1] - cdf[offset]) > 0) 
			{
				du /= (cdf[offset + 1] - cdf[offset]);
			}

			// Compute PDF for sampled offset
			if (pdf) 
				*pdf = (funcInt > 0) ? func[offset] / funcInt : 0;

			// Return $x\in{}[0,1)$ corresponding to sample
			return (offset + du) / count();
		}

		int sampleDiscrete(Float u, Float *pdf = nullptr, Float *uRemapped = nullptr) const 
		{
			// Find surrounding CDF segments and _offset_
			int offset = findInterval((int)cdf.size(), [&](int index) 
			{ 
				return cdf[index] <= u; 
			});

			if (pdf) 
				*pdf = (funcInt > 0) ? func[offset] / (funcInt * count()) : 0;
			if (uRemapped)
				*uRemapped = (u - cdf[offset]) / (cdf[offset + 1] - cdf[offset]);
			//if (uRemapped) 
			//	CHECK(*uRemapped >= 0.f && *uRemapped <= 1.f);
			return offset;
		}

		Float discretePDF(int index) const 
		{
			//CHECK(index >= 0 && index < Count());
			return func[index] / (funcInt * count());
		}

		std::vector<Float> func, cdf;
		Float funcInt;
	};

	// LightDistribution defines a general interface for classes that provide
	// probability distributions for sampling light sources at a given point in
	// space.
	class ALightDistribution 
	{
	public:
		virtual ~ALightDistribution() = default;

		// Given a point |p| in space, this method returns a (hopefully
		// effective) sampling distribution for light sources at that point.
		virtual const ADistribution1D *lookup(const AVector3f &p) const = 0;
	};

	// The simplest possible implementation of LightDistribution: this returns
	// a uniform distribution over all light sources, ignoring the provided
	// point. This approach works well for very simple scenes, but is quite
	// ineffective for scenes with more than a handful of light sources. (This
	// was the sampling method originally used for the PathIntegrator and the
	// VolPathIntegrator in the printed book, though without the
	// UniformLightDistribution class.)
	class AUniformLightDistribution : public ALightDistribution 
	{
	public:
		
		AUniformLightDistribution(const AScene &scene);

		virtual const ADistribution1D *lookup(const AVector3f &p) const override;

	private:
		std::unique_ptr<ADistribution1D> distrib;
	};

	std::unique_ptr<ALightDistribution> createLightSampleDistribution(
		const std::string &name, const AScene &scene);

}

#endif