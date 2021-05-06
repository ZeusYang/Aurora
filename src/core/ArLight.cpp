#include "ArLight.h"

#include "ArRng.h"
#include "ArScene.h"
#include "ArSampler.h"

namespace Aurora
{
	//-------------------------------------------ALight-------------------------------------

	ALight::ALight(const APropertyList &props)
	{
		m_nSamples = props.getInteger("LightSamples", 1);
	}

	ALight::ALight(int flags, const ATransform &lightToWorld, int nSamples)
		: m_flags(flags), m_nSamples(glm::max(1, nSamples)), m_lightToWorld(lightToWorld),
		m_worldToLight(inverse(lightToWorld))
	{
		//++numLights;
	}

	ALight::~ALight() {}

	ASpectrum ALight::Le(const ARay &ray) const { return ASpectrum(0.f); }

	//-------------------------------------------AVisibilityTester-------------------------------------

	bool AVisibilityTester::unoccluded(const AScene &scene) const
	{
		return !scene.hit(m_p0.spawnRayTo(m_p1));
	}

	ASpectrum AVisibilityTester::tr(const AScene &scene, ASampler &sampler) const
	{
		//ARay ray(p0.SpawnRayTo(p1));
		ASpectrum Tr(1.f);
		//while (true) {
		//	ASurfaceInteraction isect;
		//	bool hitSurface = scene.Intersect(ray, &isect);
		//	// Handle opaque surface along ray's path
		//	if (hitSurface && isect.primitive->GetMaterial() != nullptr)
		//		return ASpectrum(0.0f);

		//	// Update transmittance for current ray segment
		//	if (ray.medium) Tr *= ray.medium->Tr(ray, sampler);

		//	// Generate next ray segment or return final transmittance
		//	if (!hitSurface) break;
		//	ray = isect.SpawnRayTo(p1);
		//}
		return Tr;
	}

	//-------------------------------------------AAreaLight-------------------------------------

	AAreaLight::AAreaLight(const APropertyList &props) : ALight(props) { m_flags = (int)ALightFlags::ALightArea; }

	AAreaLight::AAreaLight(const ATransform &lightToWorld, int nSamples)
		: ALight((int)ALightFlags::ALightArea, lightToWorld, nSamples) { }

	//-------------------------------------------ADiffuseAreaLight-------------------------------------

	AURORA_REGISTER_CLASS(ADiffuseAreaLight, "AreaDiffuse")

	ADiffuseAreaLight::ADiffuseAreaLight(const APropertyTreeNode &node)
		: AAreaLight(node.getPropertyList()), m_shape(nullptr)
	{
		const auto &props = node.getPropertyList();
		AVector3f _Le = props.getVector3f("Radiance");
		Float _tmp[] = { _Le.x, _Le.y, _Le.z };
		m_Lemit = ASpectrum::fromRGB(_tmp);

		m_twoSided = props.getBoolean("TwoSided");
	}

	ADiffuseAreaLight::ADiffuseAreaLight(const ATransform &lightToWorld, const ASpectrum &Lemit,
		int nSamples, AShape* shape, bool twoSided)
		: AAreaLight(lightToWorld, nSamples), m_Lemit(Lemit), m_shape(shape),
		m_twoSided(twoSided), m_area(shape->area()) { }

	void ADiffuseAreaLight::setParent(AObject *parent)
	{
		switch (parent->getClassType())
		{
		case AClassType::AEHitable:
			m_shape = static_cast<AHitableEntity*>(parent)->getShape();
			m_area = m_shape->area();
			m_lightToWorld = m_shape->m_objectToWorld;
			m_worldToLight = m_shape->m_worldToObject;
			break;
		default:
			LOG(ERROR) << "ADiffuseAreaLight::setParent(" << getClassTypeName(parent->getClassType())
				<< ") is no supported";
			break;
		}
	}

	ASpectrum ADiffuseAreaLight::power() const
	{
		return (m_twoSided ? 2 : 1) * m_Lemit * m_area * aPi;
	}

	ASpectrum ADiffuseAreaLight::sample_Li(const AInteraction &ref, const AVector2f &u, AVector3f &wi,
		Float &pdf, AVisibilityTester &vis) const
	{
		AInteraction pShape = m_shape->sample(ref, u, pdf);

		if (pdf == 0 || lengthSquared(pShape.p - ref.p) == 0)
		{
			pdf = 0;
			return 0.f;
		}

		wi = normalize(pShape.p - ref.p);
		vis = AVisibilityTester(ref, pShape);
		return L(pShape, -wi);
	}

	Float ADiffuseAreaLight::pdf_Li(const AInteraction &ref, const AVector3f &wi) const
	{
		return m_shape->pdf(ref, wi);
	}

	ASpectrum ADiffuseAreaLight::sample_Le(const AVector2f &u1, const AVector2f &u2, ARay &ray,
		AVector3f &nLight, Float &pdfPos, Float &pdfDir) const
	{
		// Sample a point on the area light's _Shape_, _pShape_
		AInteraction pShape = m_shape->sample(u1, pdfPos);
		nLight = pShape.n;

		// Sample a cosine-weighted outgoing direction _w_ for area light
		AVector3f w;
		if (m_twoSided)
		{
			AVector2f u = u2;
			// Choose a side to sample and then remap u[0] to [0,1] before
			// applying cosine-weighted hemisphere sampling for the chosen side.
			if (u[0] < .5)
			{
				u[0] = glm::min(u[0] * 2, aOneMinusEpsilon);
				w = cosineSampleHemisphere(u);
			}
			else
			{
				u[0] = glm::min((u[0] - .5f) * 2, aOneMinusEpsilon);
				w = cosineSampleHemisphere(u);
				w.z *= -1;
			}
			pdfDir = 0.5f * cosineHemispherePdf(std::abs(w.z));
		}
		else
		{
			w = cosineSampleHemisphere(u2);
			pdfDir = cosineHemispherePdf(w.z);
		}

		AVector3f v1, v2, n(pShape.n);
		coordinateSystem(n, v1, v2);
		w = w.x * v1 + w.y * v2 + w.z * n;
		ray = pShape.spawnRay(w);
		return L(pShape, w);
	}

	void ADiffuseAreaLight::pdf_Le(const ARay &ray, const AVector3f &n, Float &pdfPos, Float &pdfDir) const
	{
		AInteraction it(ray.origin(), n, AVector3f(n));
		pdfPos = m_shape->pdf(it);
		pdfDir = m_twoSided ? (.5 * cosineHemispherePdf(absDot(n, ray.direction())))
			: cosineHemispherePdf(dot(n, ray.direction()));
	}
}