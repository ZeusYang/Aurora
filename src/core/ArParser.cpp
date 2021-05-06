#include "ArParser.h"

#include <fstream>

#include "ArFilm.h"
#include "ArSampler.h"
#include "ArFilter.h"
#include "ArShape.h"
#include "ArCamera.h"
#include "ArMaterial.h"
#include "ArLight.h"

using namespace nlohmann;

namespace Aurora
{
	void AParser::parser(
		const std::string &path,
		AScene::ptr &_scene,
		AIntegrator::ptr &_integrator)
	{
		_integrator = nullptr;
		_scene = nullptr;
		
		json _scene_json;
		{
			std::ifstream infile(path);

			if (!infile)
			{
				LOG(ERROR) << "Could not open the json file: " << path;
			}

			infile >> _scene_json;
			infile.close();
		}
		LOG(INFO) << "Parse the scene file from " << path;

		//Build property tree
		std::function<APropertyTreeNode(const std::string &tag, const json_value_type &jsonData)> build_property_tree_func;
		build_property_tree_func = [&](const std::string &tag, const json_value_type &jsonData) -> APropertyTreeNode
		{	
			APropertyTreeNode node(tag);
			if (jsonData.is_object())
			{
				for (const auto& item : jsonData.items())
				{
					const auto &key = item.key();
					const auto &value = item.value();
					if (!value.is_object())
					{
						auto get_func = [](const json_value_type &target) -> std::string
						{
							if (target.is_string())
							{
								return target.get<std::string>();
							}
							else if (target.is_boolean())
							{
								bool ret = target.get<bool>();
								return ret ? "true" : "false";
							}
							else if(target.is_number_float())
							{
								float ret = target.get<float>();
								std::stringstream ss;
								ss << ret;
								return ss.str();
							}
							else
							{
								int ret = target.get<int>();
								std::stringstream ss;
								ss << ret;
								return ss.str();
							}
						};

						std::vector<std::string> values;
						if (value.is_array())
						{
							for (int i = 0; i < value.size(); ++i)
							{
								values.push_back(get_func(value[i]));
							}
						}
						else
						{
							values.push_back(get_func(value));
						}
						node.addProperty(key, values);
					}
					else
					{
						//build-in class type, recursively build it
						node.addChild(build_property_tree_func(key, jsonData[key]));
					}

				}
			}
			return node;
		};

		//Integrator loading
		{
			if (!_scene_json.contains("Integrator"))
			{
				LOG(ERROR) << "There is no Integrator in " << path;
			}
			APropertyTreeNode integratorNode = build_property_tree_func("Integrator", _scene_json["Integrator"]);
			_integrator = AIntegrator::ptr(static_cast<AIntegrator*>(AObjectFactory::createInstance(
				integratorNode.getTypeName(), integratorNode)));
			_integrator->activate();
		}

		std::vector<ALight::ptr> _lights;
		AHitableList::ptr _aggregate = std::make_shared<AHitableList>();

		//Hitables loading
		{
			if (!_scene_json.contains("Hitables"))
			{
				LOG(ERROR) << "There is no Hitable in " << path;
			}
			const auto &hitables_json = _scene_json["Hitables"];
			for (int i = 0; i < hitables_json.size(); ++i)
			{
				APropertyTreeNode hitableNode = build_property_tree_func("Hitable", hitables_json[i]);
				AHitable::ptr hitable = AHitable::ptr(static_cast<AHitable*>(AObjectFactory::createInstance(
					hitableNode.getTypeName(), hitableNode)));
				hitable->activate();
				_aggregate->addHitable(hitable);
			}

			const auto& hitables = _aggregate->getHitableList();
			for (int i = 0; i < hitables.size(); ++i)
			{
				if (hitables[i]->getAreaLight() != nullptr)
				{
					_lights.push_back(ALight::ptr(dynamic_cast<AHitableEntity*>(hitables[i].get())->getAreaLightPtr()));
				}
			}
		}

		_scene = std::make_shared<AScene>(_aggregate, _lights);

	}

}