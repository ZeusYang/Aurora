#include "ArShape.h"

#include "ArInteraction.h"
#include "ArSampler.h"

#include <array>

namespace Aurora
{
	//-------------------------------------------AShape-------------------------------------

	AShape::AShape(const APropertyList &props)
	{
		AVector3f _trans = props.getVector3f("Translate", AVector3f(0.0f));
		AVector3f _scale = props.getVector3f("Scale", AVector3f(1.0f));
		m_objectToWorld = translate(_trans) * scale(_scale.x, _scale.y, _scale.z);
		m_worldToObject = inverse(m_objectToWorld);
	}

	AShape::AShape(const ATransform &objectToWorld, const ATransform &worldToObject)
		: m_objectToWorld(objectToWorld), m_worldToObject(worldToObject) {}

	bool AShape::hit(const ARay &ray) const
	{
		Float tHit = ray.m_tMax;
		ASurfaceInteraction isect;
		return hit(ray, tHit, isect);
	}

	ABounds3f AShape::worldBound() const { return m_objectToWorld(objectBound()); }

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

	//-------------------------------------------ASphereShape-------------------------------------

	AURORA_REGISTER_CLASS(ASphereShape, "Sphere")

	ASphereShape::ASphereShape(const APropertyTreeNode &node)
		: AShape(node.getPropertyList()), m_radius(node.getPropertyList().getFloat("Radius", 1.0f)) { activate(); }

	ASphereShape::ASphereShape(const ATransform &objectToWorld, const ATransform &worldToObject,
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
		it.n = normalize((m_objectToWorld)(pObj, 0.0f));

		pObj *= m_radius / distance(pObj, AVector3f(0, 0, 0));
		it.p = (m_objectToWorld)(pObj, 1.0f);

		pdf = 1 / area();
		return it;
	}

	AInteraction ASphereShape::sample(const AInteraction &ref, const AVector2f &u, Float &pdf) const
	{
		AVector3f pCenter = (m_objectToWorld)(AVector3f(0, 0, 0), 1.0f);

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
		AVector3f pCenter = (m_objectToWorld)(AVector3f(0, 0, 0), 1.0f);
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
		ARay ray = (m_worldToObject)(r);

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
		ARay ray = (m_worldToObject)(r);

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

		isect = (m_objectToWorld)(ASurfaceInteraction(pHit, AVector2f(u, v), -ray.direction(),
			dpdu, dpdv, this));

		isect.n = faceforward(isect.n, isect.wo);

		tHit = tShapeHit;

		return true;
	}

	Float ASphereShape::solidAngle(const AVector3f &p, int nSamples) const
	{
		AVector3f pCenter = (m_objectToWorld)(AVector3f(0, 0, 0), 1.0f);
		if (distanceSquared(p, pCenter) <= m_radius * m_radius)
			return 4 * aPi;
		Float sinTheta2 = m_radius * m_radius / distanceSquared(p, pCenter);
		Float cosTheta = glm::sqrt(glm::max((Float)0, 1 - sinTheta2));
		return (2 * aPi * (1 - cosTheta));
	}

	//-------------------------------------------ATriangleShape-------------------------------------

	AURORA_REGISTER_CLASS(ATriangleShape, "Triangle")

	ATriangleShape::ATriangleShape(const APropertyTreeNode &node)
		:AShape(node.getPropertyList())
	{
		const auto &props = node.getPropertyList();
		m_p0 = (props.getVector3f("P0"));
		m_p1 = (props.getVector3f("P1"));
		m_p2 = (props.getVector3f("P2"));
		activate();
	}

	ATriangleShape::ATriangleShape(const ATransform &objectToWorld, const ATransform &worldToObject,
		AVector3f v[3]) : AShape(objectToWorld, worldToObject), m_p0(v[0]), m_p1(v[1]), m_p2(v[2]) {}

	ABounds3f ATriangleShape::objectBound() const
	{
		// Get triangle vertices in _p0_, _p1_, and _p2_
		return unionBounds(ABounds3f((m_worldToObject)(m_p0, 1.0f), (m_worldToObject)(m_p1, 1.0f)),
			(m_worldToObject)(m_p2, 1.0f));
	}

	ABounds3f ATriangleShape::worldBound() const
	{
		const AVector3f &p0 = m_p0;
		const AVector3f &p1 = m_p1;
		const AVector3f &p2 = m_p2;
		return unionBounds(ABounds3f(p0, p1), p2);
	}

	Float ATriangleShape::area() const 
	{
		// Get triangle vertices in _p0_, _p1_, and _p2_
		const AVector3f &p0 = m_p0;
		const AVector3f &p1 = m_p1;
		const AVector3f &p2 = m_p2;
		return 0.5 * length(cross(p1 - p0, p2 - p0));
	}

	AInteraction ATriangleShape::sample(const AVector2f &u, Float &pdf) const
	{
		AVector2f b = uniformSampleTriangle(u);
		// Get triangle vertices in _p0_, _p1_, and _p2_
		const AVector3f &p0 = m_p0;
		const AVector3f &p1 = m_p1;
		const AVector3f &p2 = m_p2;
		AInteraction it;
		it.p = b[0] * p0 + b[1] * p1 + (1 - b[0] - b[1]) * p2;
		// Compute surface normal for sampled point on triangle
		it.n = normalize(AVector3f(cross(p1 - p0, p2 - p0)));

		pdf = 1 / area();
		return it;
	}

	bool ATriangleShape::hit(const ARay &ray) const
	{
		// Get triangle vertices in _p0_, _p1_, and _p2_
		const AVector3f &p0 = m_p0;
		const AVector3f &p1 = m_p1;
		const AVector3f &p2 = m_p2;

		// Perform ray--triangle intersection test

		// Transform triangle vertices to ray coordinate space

		// Translate vertices based on ray origin
		AVector3f p0t = p0 - AVector3f(ray.origin());
		AVector3f p1t = p1 - AVector3f(ray.origin());
		AVector3f p2t = p2 - AVector3f(ray.origin());

		// Permute components of triangle vertices and ray direction
		int kz = maxDimension(abs(ray.direction()));
		int kx = kz + 1;
		if (kx == 3) kx = 0;
		int ky = kx + 1;
		if (ky == 3) ky = 0;
		AVector3f d = permute(ray.direction(), kx, ky, kz);
		p0t = permute(p0t, kx, ky, kz);
		p1t = permute(p1t, kx, ky, kz);
		p2t = permute(p2t, kx, ky, kz);

		// Apply shear transformation to translated vertex positions
		Float Sx = -d.x / d.z;
		Float Sy = -d.y / d.z;
		Float Sz = 1.f / d.z;
		p0t.x += Sx * p0t.z;
		p0t.y += Sy * p0t.z;
		p1t.x += Sx * p1t.z;
		p1t.y += Sy * p1t.z;
		p2t.x += Sx * p2t.z;
		p2t.y += Sy * p2t.z;

		// Compute edge function coefficients _e0_, _e1_, and _e2_
		Float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
		Float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
		Float e2 = p0t.x * p1t.y - p0t.y * p1t.x;

		// Fall back to double precision test at triangle edges
		if (sizeof(Float) == sizeof(float) &&
			(e0 == 0.0f || e1 == 0.0f || e2 == 0.0f)) 
		{
			double p2txp1ty = (double)p2t.x * (double)p1t.y;
			double p2typ1tx = (double)p2t.y * (double)p1t.x;
			e0 = (float)(p2typ1tx - p2txp1ty);
			double p0txp2ty = (double)p0t.x * (double)p2t.y;
			double p0typ2tx = (double)p0t.y * (double)p2t.x;
			e1 = (float)(p0typ2tx - p0txp2ty);
			double p1txp0ty = (double)p1t.x * (double)p0t.y;
			double p1typ0tx = (double)p1t.y * (double)p0t.x;
			e2 = (float)(p1typ0tx - p1txp0ty);
		}

		// Perform triangle edge and determinant tests
		if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
			return false;
		Float det = e0 + e1 + e2;
		if (det == 0) 
			return false;

		// Compute scaled hit distance to triangle and test against ray $t$ range
		p0t.z *= Sz;
		p1t.z *= Sz;
		p2t.z *= Sz;
		Float tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
		if (det < 0 && (tScaled >= 0 || tScaled < ray.m_tMax * det))
			return false;
		else if (det > 0 && (tScaled <= 0 || tScaled > ray.m_tMax * det))
			return false;

		// Compute barycentric coordinates and $t$ value for triangle intersection
		Float invDet = 1 / det;
		Float b0 = e0 * invDet;
		Float b1 = e1 * invDet;
		Float b2 = e2 * invDet;
		Float t = tScaled * invDet;

		// Ensure that computed triangle $t$ is conservatively greater than zero

		// Compute $\delta_z$ term for triangle $t$ error bounds
		Float maxZt = maxComponent(abs(AVector3f(p0t.z, p1t.z, p2t.z)));
		Float deltaZ = gamma(3) * maxZt;

		// Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
		Float maxXt = maxComponent(abs(AVector3f(p0t.x, p1t.x, p2t.x)));
		Float maxYt = maxComponent(abs(AVector3f(p0t.y, p1t.y, p2t.y)));
		Float deltaX = gamma(5) * (maxXt + maxZt);
		Float deltaY = gamma(5) * (maxYt + maxZt);

		// Compute $\delta_e$ term for triangle $t$ error bounds
		Float deltaE = 2 * (gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);

		// Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
		Float maxE = maxComponent(abs(AVector3f(e0, e1, e2)));
		Float deltaT = 3 * (gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) * glm::abs(invDet);
		if (t <= deltaT)
			return false;

		return true;
	}

	bool ATriangleShape::hit(const ARay &ray, Float &tHit, ASurfaceInteraction &isect) const
	{
		// Get triangle vertices in _p0_, _p1_, and _p2_
		const AVector3f &p0 = m_p0;
		const AVector3f &p1 = m_p1;
		const AVector3f &p2 = m_p2;

		// Perform ray--triangle intersection test

		// Transform triangle vertices to ray coordinate space

		// Translate vertices based on ray origin
		AVector3f p0t = p0 - AVector3f(ray.origin());
		AVector3f p1t = p1 - AVector3f(ray.origin());
		AVector3f p2t = p2 - AVector3f(ray.origin());

		// Permute components of triangle vertices and ray direction
		int kz = maxDimension(abs(ray.direction()));
		int kx = kz + 1;
		if (kx == 3) kx = 0;
		int ky = kx + 1;
		if (ky == 3) ky = 0;
		AVector3f d = permute(ray.direction(), kx, ky, kz);
		p0t = permute(p0t, kx, ky, kz);
		p1t = permute(p1t, kx, ky, kz);
		p2t = permute(p2t, kx, ky, kz);

		// Apply shear transformation to translated vertex positions
		Float Sx = -d.x / d.z;
		Float Sy = -d.y / d.z;
		Float Sz = 1.f / d.z;
		p0t.x += Sx * p0t.z;
		p0t.y += Sy * p0t.z;
		p1t.x += Sx * p1t.z;
		p1t.y += Sy * p1t.z;
		p2t.x += Sx * p2t.z;
		p2t.y += Sy * p2t.z;

		// Compute edge function coefficients _e0_, _e1_, and _e2_
		Float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
		Float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
		Float e2 = p0t.x * p1t.y - p0t.y * p1t.x;

		// Fall back to double precision test at triangle edges
		if (sizeof(Float) == sizeof(float) &&
			(e0 == 0.0f || e1 == 0.0f || e2 == 0.0f)) 
		{
			double p2txp1ty = (double)p2t.x * (double)p1t.y;
			double p2typ1tx = (double)p2t.y * (double)p1t.x;
			e0 = (float)(p2typ1tx - p2txp1ty);
			double p0txp2ty = (double)p0t.x * (double)p2t.y;
			double p0typ2tx = (double)p0t.y * (double)p2t.x;
			e1 = (float)(p0typ2tx - p0txp2ty);
			double p1txp0ty = (double)p1t.x * (double)p0t.y;
			double p1typ0tx = (double)p1t.y * (double)p0t.x;
			e2 = (float)(p1typ0tx - p1txp0ty);
		}

		// Perform triangle edge and determinant tests
		if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
			return false;
		Float det = e0 + e1 + e2;
		if (det == 0) 
			return false;

		// Compute scaled hit distance to triangle and test against ray $t$ range
		p0t.z *= Sz;
		p1t.z *= Sz;
		p2t.z *= Sz;
		Float tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
		if (det < 0 && (tScaled >= 0 || tScaled < ray.m_tMax * det))
			return false;
		else if (det > 0 && (tScaled <= 0 || tScaled > ray.m_tMax * det))
			return false;

		// Compute barycentric coordinates and $t$ value for triangle intersection
		Float invDet = 1 / det;
		Float b0 = e0 * invDet;
		Float b1 = e1 * invDet;
		Float b2 = e2 * invDet;
		Float t = tScaled * invDet;

		// Ensure that computed triangle $t$ is conservatively greater than zero

		// Compute $\delta_z$ term for triangle $t$ error bounds
		Float maxZt = maxComponent(abs(AVector3f(p0t.z, p1t.z, p2t.z)));
		Float deltaZ = gamma(3) * maxZt;

		// Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
		Float maxXt = maxComponent(abs(AVector3f(p0t.x, p1t.x, p2t.x)));
		Float maxYt = maxComponent(abs(AVector3f(p0t.y, p1t.y, p2t.y)));
		Float deltaX = gamma(5) * (maxXt + maxZt);
		Float deltaY = gamma(5) * (maxYt + maxZt);

		// Compute $\delta_e$ term for triangle $t$ error bounds
		Float deltaE = 2 * (gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);

		// Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
		Float maxE = maxComponent(abs(AVector3f(e0, e1, e2)));
		Float deltaT = 3 * (gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) * glm::abs(invDet);
		if (t <= deltaT) 
			return false;

		// Compute triangle partial derivatives
		AVector3f dpdu, dpdv;
		AVector2f uv[3] = { AVector2f(0,0), AVector2f(1,0), AVector2f(1,1) };
		//GetUVs(uv);

		// Compute deltas for triangle partial derivatives
		AVector2f duv02 = uv[0] - uv[2], duv12 = uv[1] - uv[2];
		AVector3f dp02 = p0 - p2, dp12 = p1 - p2;
		Float determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
		bool degenerateUV = glm::abs(determinant) < 1e-8;
		if (!degenerateUV) 
		{
			Float invdet = 1 / determinant;
			dpdu = (duv12[1] * dp02 - duv02[1] * dp12) * invdet;
			dpdv = (-duv12[0] * dp02 + duv02[0] * dp12) * invdet;
		}
		if (degenerateUV || lengthSquared(cross(dpdu, dpdv)) == 0) 
		{
			// Handle zero determinant for triangle partial derivative matrix
			AVector3f ng = cross(p2 - p0, p1 - p0);
			// The triangle is actually degenerate; the intersection is bogus.
			if (lengthSquared(ng) == 0)
				return false;

			coordinateSystem(normalize(ng), dpdu, dpdv);
		}

		// Interpolate $(u,v)$ parametric coordinates and hit point
		AVector3f pHit = b0 * p0 + b1 * p1 + b2 * p2;
		AVector2f uvHit = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];

		// Fill in _SurfaceInteraction_ from triangle hit
		isect = ASurfaceInteraction(pHit, uvHit, -ray.direction(), dpdu, dpdv, this);

		// Override surface normal in _isect_ for triangle
		isect.n = AVector3f(normalize(cross(dp02, dp12)));
		tHit = t;
	
		return true;
	}

	Float ATriangleShape::solidAngle(const AVector3f &p, int nSamples) const
	{
		// Project the vertices into the unit sphere around p.
		std::array<AVector3f, 3> pSphere = { normalize(m_p0 - p), normalize(m_p1 - p), normalize(m_p2 - p) };

		// http://math.stackexchange.com/questions/9819/area-of-a-spherical-triangle
		// Girard's theorem: surface area of a spherical triangle on a unit
		// sphere is the 'excess angle' alpha+beta+gamma-pi, where
		// alpha/beta/gamma are the interior angles at the vertices.
		//
		// Given three vertices on the sphere, a, b, c, then we can compute,
		// for example, the angle c->a->b by
		//
		// cos theta =  Dot(Cross(c, a), Cross(b, a)) /
		//              (Length(Cross(c, a)) * Length(Cross(b, a))).
		//
		AVector3f cross01 = (cross(pSphere[0], pSphere[1]));
		AVector3f cross12 = (cross(pSphere[1], pSphere[2]));
		AVector3f cross20 = (cross(pSphere[2], pSphere[0]));

		// Some of these vectors may be degenerate. In this case, we don't want
		// to normalize them so that we don't hit an assert. This is fine,
		// since the corresponding dot products below will be zero.
		if (lengthSquared(cross01) > 0) cross01 = normalize(cross01);
		if (lengthSquared(cross12) > 0) cross12 = normalize(cross12);
		if (lengthSquared(cross20) > 0) cross20 = normalize(cross20);

		// We only need to do three cross products to evaluate the angles at
		// all three vertices, though, since we can take advantage of the fact
		// that Cross(a, b) = -Cross(b, a).
		return glm::abs(
			glm::acos(clamp(dot(cross01, -cross12), -1, 1)) +
			glm::acos(clamp(dot(cross12, -cross20), -1, 1)) +
			glm::acos(clamp(dot(cross20, -cross01), -1, 1)) - aPi);
	}
}