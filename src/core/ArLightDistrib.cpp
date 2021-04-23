#include "ArLightDistrib.h"

#include "ArScene.h"

namespace Aurora
{
	std::unique_ptr<ALightDistribution> createLightSampleDistribution(
		const std::string &name, const AScene &scene) 
	{
		//if (name == "uniform" || scene.m_lights.size() == 1)
		{
			return std::unique_ptr<ALightDistribution>{
				new AUniformLightDistribution(scene)};
		}
		//else if (name == "power")
		//{
		//	return std::unique_ptr<ALightDistribution>{
		//		new APowerLightDistribution(scene)};
		//}
		//else if (name == "spatial")
		//{
		//	return std::unique_ptr<ALightDistribution>{
		//		new ASpatialLightDistribution(scene)};
		//}
		//else 
		//{
		//	Error(
		//		"Light sample distribution type \"%s\" unknown. Using \"spatial\".",
		//		name.c_str());
		//	return std::unique_ptr<LightDistribution>{new SpatialLightDistribution(scene)};
		//}
	}

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