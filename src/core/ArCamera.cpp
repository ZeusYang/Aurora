#include "ArCamera.h"

namespace Aurora
{
	//-------------------------------------------ACamera-------------------------------------

	ACamera::ACamera(const ATransform &cameraToWorld, AFilm::ptr film)
		:m_cameraToWorld(cameraToWorld), m_film(film) {}

	ACamera::~ACamera() {}

	//-------------------------------------------APerspectiveCamera-------------------------------------

	APerspectiveCamera::APerspectiveCamera(
		const ATransform &cameraToWorld,
		const ABounds2f &screenWindow,
		Float fov,
		AFilm::ptr film)
		: AProjectiveCamera(cameraToWorld, perspective(fov, 1e-2f, 1000.f), screenWindow, film)
	{
		// Compute image plane bounds at $z=1$ for _PerspectiveCamera_
		AVector2i res = film->getResolution();
		AVector3f pMin = m_rasterToCamera(AVector3f(0, 0, 0), 1.0f);
		AVector3f pMax = m_rasterToCamera(AVector3f(res.x, res.y, 0), 1.0f);
		pMin /= pMin.z;
		pMax /= pMax.z;
		A = std::abs((pMax.x - pMin.x) * (pMax.y - pMin.y));
	}

	Float APerspectiveCamera::rayCasting(const ACameraSample &sample, ARay &ray) const
	{
		// Compute raster and camera sample positions
		AVector3f pFilm = AVector3f(sample.pFilm.x, sample.pFilm.y, 0);
		AVector3f pCamera = m_rasterToCamera(pFilm, 1.0f);
		ray = ARay(AVector3f(0, 0, 0), normalize(AVector3f(pCamera)));
		ray = m_cameraToWorld(ray);
		return 1.f;
	}
}