#include "ArShape.h"

#include "ArInteraction.h"
#include "ArSampler.h"

namespace Aurora
{
	AShape::AShape(const APropertyList &props) {}

	AShape::AShape(ATransform *objectToWorld, ATransform *worldToObject)
		: m_objectToWorld(objectToWorld), m_worldToObject(worldToObject) {}

	bool AShape::hit(const ARay &ray) const
	{
		Float tHit = ray.m_tMax;
		ASurfaceInteraction isect;
		return hit(ray, tHit, isect);
	}

	void AShape::setTransform(ATransform *objectToWorld, ATransform *worldToObject)
	{
		m_objectToWorld = objectToWorld;
		m_worldToObject = worldToObject;
	}

	ABounds3f AShape::worldBound() const { return (*m_objectToWorld)(objectBound()); }

	AInteraction AShape::sample(const AInteraction &ref, const AVector2f &u, Float &pdf) const
	{
		// Sample a point on the shape given a reference point |ref| and
		// return the PDF with respect to solid angle from |ref|.
		AInteraction intr = sample(u, pdf);
		AVector3f wi = intr.p - ref.p;
		if (dot(wi, wi) == 0)
		{
			pdf = 0;
		}
		else
		{
			wi = normalize(wi);
			// Convert from area measure, as returned by the Sample() call
			// above, to solid angle measure.
			pdf *= distanceSquared(ref.p, intr.p) / absDot(intr.n, -wi);
			if (std::isinf(pdf))
				pdf = 0.f;
		}
		return intr;
	}

	Float AShape::pdf(const AInteraction &ref, const AVector3f &wi) const
	{
		// Intersect sample ray with area light geometry
		ARay ray = ref.spawnRay(wi);
		Float tHit;
		ASurfaceInteraction isectLight;
		// Ignore any alpha textures used for trimming the shape when performing
		// this intersection. Hack for the "San Miguel" scene, where this is used
		// to make an invisible area light.
		if (!hit(ray, tHit, isectLight))
			return 0;

		// Convert light sample weight to solid angle measure
		Float pdf = distanceSquared(ref.p, isectLight.p) / (absDot(isectLight.n, -wi) * area());
		if (std::isinf(pdf))
			pdf = 0.f;
		return pdf;
	}

	Float AShape::solidAngle(const AVector3f &p, int nSamples) const
	{
		//AInteraction ref(p, AVector3f(), AVector3f(), AVector3f(0, 0, 1), 0);
		//double solidAngle = 0;
		//for (int i = 0; i < nSamples; ++i) 
		//{
		//	AVector2f u{ radicalInverse(0, i), radicalInverse(1, i) };
		//	Float pdf;
		//	AInteraction pShape = sample(ref, u, pdf);
		//	if (pdf > 0 && !hit(ARay(p, pShape.p - p, .999f))) 
		//	{
		//		solidAngle += 1 / pdf;
		//	}
		//}
		//return solidAngle / nSamples;
		return 1.0f;
	}

}