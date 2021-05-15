#include "ArWhittedIntegrator.h"

#include "ArBSDF.h"
#include "ArScene.h"

namespace Aurora
{
	AURORA_REGISTER_CLASS(AWhittedIntegrator, "Whitted")

	AWhittedIntegrator::AWhittedIntegrator(const APropertyTreeNode &node)
		:ASamplerIntegrator(nullptr, nullptr), m_maxDepth(node.getPropertyList().getInteger("Depth", 2))
	{
		//Sampler
		{
			const auto &samplerNode = node.getPropertyChild("Sampler");
			m_sampler = ASampler::ptr(static_cast<ASampler*>(AObjectFactory::createInstance(
				samplerNode.getTypeName(), samplerNode)));
		}
		//Camera
		{
			const auto &cameraNode = node.getPropertyChild("Camera");
			m_camera = ACamera::ptr(static_cast<ACamera*>(AObjectFactory::createInstance(
				cameraNode.getTypeName(), cameraNode)));
		}

		activate();
	}

	ASpectrum AWhittedIntegrator::Li(const ARay &ray, const AScene &scene,
		ASampler &sampler, MemoryArena &arena, int depth) const
	{
		ASpectrum L(0.);

		ASurfaceInteraction isect;

		// No intersection found, just return lights emission
		if (!scene.hit(ray, isect))
		{
			for (const auto &light : scene.m_lights)
				L += light->Le(ray);
			return L;
		}

		// Compute emitted and reflected light at ray intersection point

		// Initialize common variables for Whitted integrator
		const AVector3f &n = isect.n;
		AVector3f wo = isect.wo;

		// Calculate BSDF function for surface interaction
		isect.computeScatteringFunctions(ray, arena);

		// There is no bsdf funcion
		if (!isect.bsdf)
		{
			return Li(isect.spawnRay(ray.direction()), scene, sampler, arena, depth);
		}

		// Compute emitted light if ray hit an area light source -> Le (emission term)
		L += isect.Le(wo);

		// Add contribution of each light source -> shadow ray
		for (const auto &light : scene.m_lights)
		{
			AVector3f wi;
			Float pdf;
			AVisibilityTester visibility;
			ASpectrum Li = light->sample_Li(isect, sampler.get2D(), wi, pdf, visibility);

			if (Li.isBlack() || pdf == 0)
				continue;

			ASpectrum f = isect.bsdf->f(wo, wi);
			if (!f.isBlack() && visibility.unoccluded(scene))
			{
				L += f * Li * absDot(wi, n) / pdf;
			}
		}

		if (depth + 1 < m_maxDepth)
		{
			// Trace rays for specular reflection and refraction
			L += specularReflect(ray, isect, scene, sampler, arena, depth);
			L += specularTransmit(ray, isect, scene, sampler, arena, depth);
		}

		return L;
	}
}