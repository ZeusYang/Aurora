#include "ArSphereShape.h"

#include "ArInteraction.h"
#include "ArSampler.h"

namespace Aurora
{
	//-------------------------------------------ASphereShape-------------------------------------

	AURORA_REGISTER_CLASS(ASphereShape, "Sphere")

		ASphereShape::ASphereShape(const APropertyTreeNode &node)
		: AShape(node.getPropertyList()), m_radius(node.getPropertyList().getFloat("Radius", 1.0f)) {
		activate();
	}

	ASphereShape::ASphereShape(ATransform *objectToWorld, ATransform *worldToObject,
		const float radius) : AShape(objectToWorld, worldToObject), m_radius(radius) {}

	ABounds3f ASphereShape::objectBound() const
	{
		return ABounds3f(AVector3f(-m_radius, -m_radius, -m_radius),
			AVector3f(m_radius, m_radius, m_radius));
	}

	Float ASphereShape::area() const { return 4.0f * aPi * m_radius * m_radius; }

	AInteraction ASphereShape::sample(const AVector2f &u, Float &pdf) const
	{
		AVector3f pObj = AVector3f(0, 0, 0) + m_radius * uniformSampleSphere(u);

		AInteraction it;
		it.n = normalize((*m_objectToWorld)(pObj, 0.0f));

		pObj *= m_radius / distance(pObj, AVector3f(0, 0, 0));
		it.p = (*m_objectToWorld)(pObj, 1.0f);

		pdf = 1 / area();
		return it;
	}

	AInteraction ASphereShape::sample(const AInteraction &ref, const AVector2f &u, Float &pdf) const
	{
		AVector3f pCenter = (*m_objectToWorld)(AVector3f(0, 0, 0), 1.0f);

		// Sample uniformly on sphere if $\pt{}$ is inside it
		AVector3f pOrigin = ref.p;
		if (distanceSquared(pOrigin, pCenter) <= m_radius * m_radius)
		{
			AInteraction intr = sample(u, pdf);
			AVector3f wi = intr.p - ref.p;
			if (dot(wi, wi) == 0)
			{
				pdf = 0;
			}
			else
			{
				// Convert from area measure returned by Sample() call above to
				// solid angle measure.
				wi = normalize(wi);
				pdf *= distanceSquared(ref.p, intr.p) / absDot(intr.n, -wi);
			}
			if (std::isinf(pdf))
				pdf = 0.f;
			return intr;
		}

		// Sample sphere uniformly inside subtended cone

		// Compute coordinate system for sphere sampling
		Float dc = distance(ref.p, pCenter);
		Float invDc = 1 / dc;
		AVector3f wc = (pCenter - ref.p) * invDc;
		AVector3f wcX, wcY;
		coordinateSystem(wc, wcX, wcY);

		// Compute $\theta$ and $\phi$ values for sample in cone
		Float sinThetaMax = m_radius * invDc;
		Float sinThetaMax2 = sinThetaMax * sinThetaMax;
		Float invSinThetaMax = 1 / sinThetaMax;
		Float cosThetaMax = glm::sqrt(glm::max((Float)0.f, 1.0f - sinThetaMax2));

		Float cosTheta = (cosThetaMax - 1) * u[0] + 1;
		Float sinTheta2 = 1 - cosTheta * cosTheta;

		if (sinThetaMax2 < 0.00068523f /* sin^2(1.5 deg) */)
		{
			/* Fall back to a Taylor series expansion for small angles, where
			   the standard approach suffers from severe cancellation errors */
			sinTheta2 = sinThetaMax2 * u[0];
			cosTheta = glm::sqrt(1 - sinTheta2);
		}

		// Compute angle $\alpha$ from center of sphere to sampled point on surface
		Float cosAlpha = sinTheta2 * invSinThetaMax +
			cosTheta * glm::sqrt(glm::max((Float)0.f, 1.f - sinTheta2 * invSinThetaMax * invSinThetaMax));
		Float sinAlpha = glm::sqrt(glm::max((Float)0.f, 1.f - cosAlpha * cosAlpha));
		Float phi = u[1] * 2 * aPi;

		// Compute surface normal and sampled point on sphere
		AVector3f nWorld = sphericalDirection(sinAlpha, cosAlpha, phi, -wcX, -wcY, -wc);
		AVector3f pWorld = pCenter + m_radius * AVector3f(nWorld.x, nWorld.y, nWorld.z);

		// Return _Interaction_ for sampled point on sphere
		AInteraction it;
		it.p = pWorld;
		it.n = nWorld;

		// Uniform cone PDF.
		pdf = 1 / (2 * aPi * (1 - cosThetaMax));

		return it;
	}

	Float ASphereShape::pdf(const AInteraction &ref, const AVector3f &wi) const
	{
		AVector3f pCenter = (*m_objectToWorld)(AVector3f(0, 0, 0), 1.0f);
		// Return uniform PDF if point is inside sphere
		AVector3f pOrigin = ref.p;
		if (distanceSquared(pOrigin, pCenter) <= m_radius * m_radius)
			return AShape::pdf(ref, wi);

		// Compute general sphere PDF
		Float sinThetaMax2 = m_radius * m_radius / distanceSquared(ref.p, pCenter);
		Float cosThetaMax = glm::sqrt(glm::max((Float)0, 1 - sinThetaMax2));
		return uniformConePdf(cosThetaMax);
	}

	bool ASphereShape::hit(const ARay &r) const
	{
		Float phi;
		AVector3f pHit;

		// Transform Ray to object space
		ARay ray = (*m_worldToObject)(r);

		Float a = dot(ray.direction(), ray.direction());
		Float b = dot(ray.origin(), ray.direction());
		Float c = dot(ray.origin(), ray.origin()) - m_radius * m_radius;

		// discriminant
		Float discriminant = b * b - a * c;
		if (discriminant <= 0)
			return false;

		Float discr_sqrt = glm::sqrt(discriminant);
		Float t0 = (-b - discr_sqrt) / a;
		Float t1 = (-b + discr_sqrt) / a;

		if (t0 > t1)
			std::swap(t0, t1);

		if (t0 > ray.m_tMax || t1 <= 0)
			return false;

		Float tShapeHit = t0;
		if (tShapeHit <= 0)
		{
			tShapeHit = t1;
			if (tShapeHit > ray.m_tMax)
				return false;
		}

		return true;
	}

	bool ASphereShape::hit(const ARay &r, Float &tHit, ASurfaceInteraction &isect) const
	{
		Float phi;
		AVector3f pHit;

		// Transform Ray to object space
		ARay ray = (*m_worldToObject)(r);

		Float a = dot(ray.direction(), ray.direction());
		Float b = dot(ray.origin(), ray.direction());
		Float c = dot(ray.origin(), ray.origin()) - m_radius * m_radius;

		// discriminant
		Float discriminant = b * b - a * c;
		if (discriminant <= 0)
			return false;

		Float discr_sqrt = glm::sqrt(discriminant);
		Float t0 = (-b - discr_sqrt) / a;
		Float t1 = (-b + discr_sqrt) / a;

		if (t0 > t1)
			std::swap(t0, t1);

		if (t0 > ray.m_tMax || t1 <= 0)
			return false;

		Float tShapeHit = t0;
		if (tShapeHit <= 0)
		{
			tShapeHit = t1;
			if (tShapeHit > ray.m_tMax)
				return false;
		}

		pHit = ray((Float)tShapeHit);

		// Refine sphere intersection point
		pHit *= m_radius / distance(pHit, AVector3f(0, 0, 0));
		if (pHit.x == 0 && pHit.y == 0)
			pHit.x = 1e-5f * m_radius;

		phi = std::atan2(pHit.y, pHit.x);

		if (phi < 0)
			phi += 2 * aPi;

		Float theta = std::acos(clamp(pHit.z / m_radius, -1, 1));

		Float u = phi / (aPi * 2);
		Float v = (theta + aPiOver2) / aPi;

		// Compute sphere $\dpdu$ and $\dpdv$
		Float zRadius = glm::sqrt(pHit.x * pHit.x + pHit.y * pHit.y);
		Float invZRadius = 1 / zRadius;
		Float cosPhi = pHit.x * invZRadius;
		Float sinPhi = pHit.y * invZRadius;
		AVector3f dpdu(-2 * aPi * pHit.y, 2 * aPi * pHit.x, 0);
		AVector3f dpdv = 2 * aPi * AVector3f(pHit.z * cosPhi, pHit.z * sinPhi, -m_radius * glm::sin(theta));

		isect = (*m_objectToWorld)(ASurfaceInteraction(pHit, AVector2f(u, v), -ray.direction(),
			dpdu, dpdv, this));

		isect.n = faceforward(isect.n, isect.wo);

		tHit = tShapeHit;

		return true;
	}

	Float ASphereShape::solidAngle(const AVector3f &p, int nSamples) const
	{
		AVector3f pCenter = (*m_objectToWorld)(AVector3f(0, 0, 0), 1.0f);
		if (distanceSquared(p, pCenter) <= m_radius * m_radius)
			return 4 * aPi;
		Float sinTheta2 = m_radius * m_radius / distanceSquared(p, pCenter);
		Float cosTheta = glm::sqrt(glm::max((Float)0, 1 - sinTheta2));
		return (2 * aPi * (1 - cosTheta));
	}
}