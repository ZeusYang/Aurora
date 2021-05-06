#ifndef ARPARSER_H
#define ARPARSER_H

#include "ArAurora.h"
#include "ArMathUtils.h"
#include "ArIntegrator.h"
#include "ArScene.h"

#include "nlohmann/json.hpp"

namespace Aurora
{
	class AParser final
	{
	public:

		static void parser(const std::string &path, AScene::ptr &_scene, AIntegrator::ptr &integrator);

	private:
		using json_value_type = nlohmann::basic_json<>::value_type;

	};
}

#endif