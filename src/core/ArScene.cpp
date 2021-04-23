#include "ArScene.h"

namespace Aurora
{
	bool AScene::hit(const ARay &ray, ASurfaceInteraction &isect) const
	{
		return m_aggreShape->hit(ray, isect);
	}

	bool AScene::hit(const ARay &ray) const
	{
		return m_aggreShape->hit(ray);
	}

	bool AScene::hitTr(ARay ray, ASampler &sampler, ASurfaceInteraction &isect, ASpectrum &Tr) const
	{
		Tr = ASpectrum(1.f);
		//while (true) {
		//	bool hitSurface = hit(ray, isect);
		//	// Accumulate beam transmittance for ray segment
		//	if (ray.medium) *Tr *= ray.medium->Tr(ray, sampler);

		//	// Initialize next ray segment or terminate transmittance computation
		//	if (!hitSurface) return false;
		//	if (isect->primitive->GetMaterial() != nullptr) return true;
		//	ray = isect->SpawnRay(ray.d);
		//}
		return false;
	}
}