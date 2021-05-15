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

}