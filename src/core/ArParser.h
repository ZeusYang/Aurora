#ifndef ARPARSER_H
#define ARPARSER_H

#include "ArAurora.h"
#include "ArMathUtils.h"
#include "ArIntegrator.h"
#include "ArScene.h"
#include "ArFilm.h"
#include "ArSampler.h"
#include "ArFilter.h"
#include "ArShape.h"
#include "ArCamera.h"
#include "ArMaterial.h"
#include "ArLight.h"

#include "json.hpp" //nlohmann_json

namespace Aurora
{
	class AParser final
	{
	public:

		static void parser(
			const std::string &path,
			AScene::ptr &_scene,
			ASampler::ptr &_sampler,
			ASamplerIntegrator::ptr &integrator);

	private:
		using json_value_type = nlohmann::basic_json<>::value_type;

		static int fetchFromJson(const json_value_type &handler, const std::string &tok,
			const int &default_value);

		static Float fetchFromJson(const json_value_type &handler, const std::string &tok,
			const Float &default_value);

		static std::string fetchFromJson(const json_value_type &handler, const std::string &tok,
			const std::string &default_value);
		
		static AVector2f fetchFromJson(const json_value_type &handler, const std::string &tok,
			const AVector2f &default_value);
		
		static AVector3f fetchFromJson(const json_value_type &handler, const std::string &tok,
			const AVector3f &default_value);
		
		static std::vector<Float> fetchFromJson(const json_value_type &handler, const std::string &tok,
			const std::vector<Float> &default_value, const int &nComponents);

	};
}

#endif