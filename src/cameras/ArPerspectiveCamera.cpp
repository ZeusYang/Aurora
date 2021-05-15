#include "ArPerspectiveCamera.h"

namespace Aurora
{
	AURORA_REGISTER_CLASS(APerspectiveCamera, "Perspective")

	APerspectiveCamera::APerspectiveCamera(const APropertyTreeNode &node)
	{
		const auto props = node.getPropertyList();
		Float _fov = props.getFloat("Fov");
		auto _eye = props.getVector3f("Eye");
		auto _focus = props.getVector3f("Focus");
		auto _up = props.getVector3f("WorldUp", AVector3f(0.f, 1.f, 0.f));
		m_cameraToWorld = inverse(lookAt(_eye, _focus, _up));
		m_cameraToScreen = perspective(_fov, 1e-2f, 1000.f);

		//Film
		{
			const auto &filmNode = node.getPropertyChild("Film");
			m_film = AFilm::ptr(static_cast<AFilm*>(AObjectFactory::createInstance(
				filmNode.getTypeName(), filmNode)));
		}

		activate();
	}

	APerspectiveCamera::APerspectiveCamera(const ATransform &cameraToWorld, Float fov, AFilm::ptr film)
		: AProjectiveCamera(cameraToWorld, perspective(fov, 1e-2f, 1000.f), film)
	{
		initialize();
	}

	void APerspectiveCamera::initialize()
	{
		// Compute image plane bounds at $z=1$ for _PerspectiveCamera_
		AVector2i res = m_film->getResolution();
		AVector3f pMin = m_rasterToCamera(AVector3f(0, 0, 0), 1.0f);
		AVector3f pMax = m_rasterToCamera(AVector3f(res.x, res.y, 0), 1.0f);
		pMin /= pMin.z;
		pMax /= pMax.z;
		A = glm::abs((pMax.x - pMin.x) * (pMax.y - pMin.y));

		AProjectiveCamera::initialize();
	}

	Float APerspectiveCamera::castingRay(const ACameraSample &sample, ARay &ray) const
	{
		// Compute raster and camera sample positions
		AVector3f pFilm = AVector3f(sample.pFilm.x, sample.pFilm.y, 0);
		AVector3f pCamera = m_rasterToCamera(pFilm, 1.0f);
		ray = ARay(AVector3f(0, 0, 0), normalize(AVector3f(pCamera)));
		ray = m_cameraToWorld(ray);
		return 1.f;
	}
}