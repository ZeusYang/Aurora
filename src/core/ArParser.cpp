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
			else if (target.is_number_float())
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

		//Setup directory path
		{
			size_t last_slash_idx = path.rfind('\\');
			if (last_slash_idx == std::string::npos)
			{
				last_slash_idx = path.rfind('/');
			}
			if (last_slash_idx != std::string::npos)
			{
				APropertyTreeNode::m_directory = path.substr(0, last_slash_idx + 1);
			}
		}

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
		}

		std::vector<ALight::ptr> _lights;
		std::vector<AHitable::ptr> _hitables;
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
				_hitables.push_back(hitable);
				auto _mesh = dynamic_cast<AHitableMesh*>(hitable.get());
				// Note: if it's a mesh, add each HitableEntity to aggregate data structure instead of 
				//       treating the whole mesh as a single HitableEntity
				if (_mesh != nullptr)
				{
					const auto &triangles = _mesh->getTriangles();
					for (const auto &tri : triangles)
					{
						_aggregate->addHitable(tri);
					}
				}
				else
				{
					_aggregate->addHitable(hitable);
				}
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

		_scene = std::make_shared<AScene>(_hitables, _aggregate, _lights);

	}

}