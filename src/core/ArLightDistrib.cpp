#include "ArLightDistrib.h"

#include "ArScene.h"

namespace Aurora
{
	AUniformLightDistribution::AUniformLightDistribution(const AScene &scene) 
	{
		std::vector<Float> prob(scene.m_lights.size(), Float(1));
		distrib.reset(new ADistribution1D(&prob[0], int(prob.size())));
	}

	const ADistribution1D *AUniformLightDistribution::lookup(const AVector3f &p) const 
	{
		return distrib.get();
	}
}