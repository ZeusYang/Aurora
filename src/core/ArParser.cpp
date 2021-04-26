#include "ArParser.h"

#include <fstream>

using namespace nlohmann;

namespace Aurora
{
	void AParser::parser(
		const std::string &path,
		AScene::ptr &_scene,
		ASampler::ptr &_sampler,
		ASamplerIntegrator::ptr &_integrator)
	{
		_integrator = nullptr;
		_scene = nullptr;
		_sampler = nullptr;

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

		//Property missing in-case logging
		auto missingLog_in_case = [path](const json_value_type &handler, const std::string &token)
		{
			if (!handler.contains(token))
			{
				LOG(ERROR) << "No "<< token <<" setting in the json file: " << path;
			}
		};

		//Parsing camera setting
		ACamera::ptr _camera = nullptr;
		std::string token_camera = "Camera";
		int _width, _height;
		{
			missingLog_in_case(_scene_json, token_camera);

			auto type = fetchFromJson(_scene_json[token_camera], "Type", "Perspective");
			Float _fov = fetchFromJson(_scene_json[token_camera], "Fov", 45.0f);
			AVector3f _eye = fetchFromJson(_scene_json[token_camera], "Eye", AVector3f(0.f));
			AVector3f _focus = fetchFromJson(_scene_json[token_camera], "Focus", AVector3f(0.f));
			AVector3f _worldUp = fetchFromJson(_scene_json[token_camera], "WorldUp", AVector3f(0.f, 1.f, 0.f));

			//Parsing film setting
			AFilm::ptr _film = nullptr;
			{
				std::string token_film = "Film";
				missingLog_in_case(_scene_json[token_camera], token_film);

				_width = fetchFromJson(_scene_json[token_camera][token_film], "Width", 128);
				_height = fetchFromJson(_scene_json[token_camera][token_film], "Height", 128);

				std::string _filename;
				_filename = fetchFromJson(_scene_json[token_camera][token_film], "Filename", "rendered.png");

				std::vector<Float> cropWindow = fetchFromJson(_scene_json[token_camera][token_film],
					"CropWindow", {0, 0, 1, 1}, 4);
				ABounds2f _cropWindow(AVector2f(cropWindow[0], cropWindow[1]), AVector2f(cropWindow[2], cropWindow[3]));

				//Parsing filter setting
				std::unique_ptr<AFilter> _filter = nullptr;
				{
					std::string token_filter = "Filter";
					missingLog_in_case(_scene_json[token_camera][token_film], token_filter);
					auto type = fetchFromJson(_scene_json[token_camera][token_film][token_filter], "Type", "Box");
					auto _radius = fetchFromJson(_scene_json[token_camera][token_film][token_filter], "Radius", AVector2f(0.5f));
					if (type == "Box")
					{
						_filter = std::move(std::unique_ptr<AFilter>(new ABoxFilter(_radius)));
					}
					else
					{
						LOG(ERROR) << "Undefined filter type: " << type;
					}
				}

				_film = std::make_shared<AFilm>(AVector2i(_width, _height), _cropWindow, std::move(_filter), _filename);
			}

			if (type == "Perspective")
			{
				ABounds2f screen;
				auto res = _film->getResolution();
				Float frame = (Float)(res.x) / res.y;
				if (frame > 1.f)
				{
					screen.m_pMin.x = -frame;
					screen.m_pMax.x = frame;
					screen.m_pMin.y = -1.f;
					screen.m_pMax.y = 1.f;
				}
				else
				{
					screen.m_pMin.x = -1.f;
					screen.m_pMax.x = 1.f;
					screen.m_pMin.y = -1.f / frame;
					screen.m_pMax.y = 1.f / frame;
				}
				auto cameraToWorld = inverse(lookAt(_eye, _focus, _worldUp));
				_camera = std::make_shared<APerspectiveCamera>(cameraToWorld, screen, _fov, _film);
			}
			else
			{
				LOG(ERROR) << "Undefined camera type: " << type;
			}

		}

		//Parsing integrator setting
		std::string token_integrator = "Integrator";
		{
			missingLog_in_case(_scene_json, token_integrator);
			
			auto type = fetchFromJson(_scene_json[token_integrator], "Type", "Whitted");
			int _depth = fetchFromJson(_scene_json[token_integrator], "Depth", 4);

			std::string token_sampler = "Sampler";
			{
				missingLog_in_case(_scene_json[token_integrator], token_sampler);

				auto type = fetchFromJson(_scene_json[token_integrator][token_sampler], "Type", "Random");
				int _spp = fetchFromJson(_scene_json[token_integrator][token_sampler], "SPP", 8);
				if (type == "Random")
				{
					_sampler = std::make_shared<ARandomSampler>(_spp);
				}
				else
				{
					LOG(ERROR) << "Undefined sampler type: " << type;
				}
			}

			ABounds2i pixelBound(AVector2i(0, 0), AVector2i(_width, _height));
			if (type == "Whitted")
			{
				_integrator = std::make_shared<AWhittedIntegrator>(_depth, _camera, _sampler, pixelBound);
			}
			else
			{
				LOG(ERROR) << "Undefined integrator type: " << type;
			}
		}

		//Parsing shape setting
		std::vector<ALight::ptr> _lights;
		AHitableList::ptr _aggregate = std::make_shared<AHitableList>();
		auto parsing_shape = [&](const json_value_type &handler) -> AHitable::ptr
		{
			//Parsing material setting
			AMaterial::ptr _material = nullptr;
			std::string token_material = "Material";
			{
				missingLog_in_case(handler, token_material);
				auto type = fetchFromJson(handler[token_material], "Type", "Lambertian");
				AVector3f R = fetchFromJson(handler[token_material], "R", AVector3f(1.f));
				if (type == "Lambertian")
				{
					Float _R[3] = { R.x, R.y, R.z };
					_material = std::make_shared<ALambertianMaterial>(ASpectrum::fromRGB(_R));
				}
				else if (type == "Mirror")
				{
					Float _R[3] = { R.x, R.y, R.z };
					_material = std::make_shared<AMirrorMaterial>(ASpectrum::fromRGB(_R));
				}
				else
				{
					LOG(ERROR) << "Undefined material type: " << type;
				}

			}

			//Parsing hitable setting
			AShape::ptr _shape = nullptr;
			auto type = fetchFromJson(handler, "Type", "Sphere");
			AVector3f _translate = fetchFromJson(handler, "Translate", AVector3f(0.f));
			AVector3f _scale = fetchFromJson(handler, "Scale", AVector3f(1.f));
			ATransform _tranformation = translate(_translate) * scale(_scale.x, _scale.y, _scale.z);
			if (type == "Sphere")
			{
				Float _radius = fetchFromJson(handler, "Radius", 1.f);
				_shape = std::make_shared<ASphereShape>(_tranformation, inverse(_tranformation), _radius);
			}
			else if (type == "Triangle")
			{
				AVector3f _p[3];
				_p[0] = fetchFromJson(handler, "P0", AVector3f(0.f));
				_p[1] = fetchFromJson(handler, "P1", AVector3f(0.f));
				_p[2] = fetchFromJson(handler, "P2", AVector3f(0.f));
				_shape = std::make_shared<ATriangleShape>(_tranformation, inverse(_tranformation), _p);
			}
			else
			{
				LOG(ERROR) << "Undefined shape type: " << type;
			}
			

			//Parsing area light setting
			AAreaLight::ptr _light = nullptr;
			std::string token_light = "AreaLight";
			{
				if (handler.contains(token_light))
				{
					auto type = fetchFromJson(handler[token_light], "Type", "Diffuse");
					int _lightSamples = fetchFromJson(handler[token_light], "LightSamples", 1);
					if (type == "Diffuse")
					{
						AVector3f R = fetchFromJson(handler[token_light], "Radiance", AVector3f(1.f));
						Float _radiance[3] = { R.x, R.y, R.z };
						_light = std::make_shared<ADiffuseAreaLight>(_tranformation, ASpectrum::fromRGB(_radiance),
							_lightSamples, _shape);
					}
					else
					{
						LOG(ERROR) << "Undefined shape type: " << type;
					}
				}
			}

			if (_light != nullptr)
			{
				_lights.push_back(_light);
			}

			AHitableEntity::ptr _hitable = std::make_shared<AHitableEntity>(_shape, _material, _light);

			return _hitable;
		};

		missingLog_in_case(_scene_json, "Shapes");
		for (const auto &shape : _scene_json["Shapes"])
		{
			_aggregate->addHitable(parsing_shape(shape));
		}

		_scene = std::make_shared<AScene>(_aggregate, _lights);

		if (_scene != nullptr && _integrator != nullptr && _sampler != nullptr)
		{
			LOG(INFO) << "Successfully parse the json file: " << path;
		}
		else
		{
			LOG(FATAL) << "Failed to parse the json file: " << path;
		}
	}

	int AParser::fetchFromJson(const json_value_type &handler, const std::string &tok,
		const int &default_value)
	{
		if (!handler.contains(tok))
		{
			LOG(WARNING) << "Using default value for " << tok;
			return default_value;
		}
		return handler[tok].get<int>();
	}

	Float AParser::fetchFromJson(const json_value_type &handler, const std::string &tok,
		const Float &default_value)
	{
		if (!handler.contains(tok))
		{
			LOG(WARNING) << "Using default value for " << tok;
			return default_value;
		}
		return handler[tok].get<Float>();
	}

	std::string AParser::fetchFromJson(const json_value_type &handler, const std::string &tok,
		const std::string &default_value)
	{
		if (!handler.contains(tok))
		{
			LOG(WARNING) << "Using default value for " << tok;
			return default_value;
		}
		return handler[tok].get<std::string>();
	}

	AVector2f AParser::fetchFromJson(const json_value_type &handler, const std::string &tok,
		const AVector2f &default_value)
	{
		if (!handler.contains(tok))
		{
			LOG(WARNING) << "Using default value for " << tok;
			return default_value;
		}
		AVector2f value;
		CHECK_EQ(handler[tok].size(), 2);
		value.x = handler[tok][0].get<Float>();
		value.y = handler[tok][1].get<Float>();
		return value;
	}

	AVector3f AParser::fetchFromJson(const json_value_type &handler, const std::string &tok,
		const AVector3f &default_value)
	{
		if (!handler.contains(tok))
		{
			LOG(WARNING) << "Using default value for " << tok;
			return default_value;
		}
		AVector3f value;
		CHECK_EQ(handler[tok].size(), 3);
		value.x = handler[tok][0].get<Float>();
		value.y = handler[tok][1].get<Float>();
		value.z = handler[tok][2].get<Float>();
		return value;
	}

	std::vector<Float> AParser::fetchFromJson(const json_value_type &handler, const std::string &tok,
		const std::vector<Float> &default_value, const int &nComponents)
	{
		if (!handler.contains(tok))
		{
			LOG(WARNING) << "Using default value for " << tok;
			return default_value;
		}
		CHECK_EQ(handler[tok].size(), nComponents);
		std::vector<Float> value;
		for (int i = 0; i < nComponents; ++i)
		{
			value.push_back(handler[tok][i].get<Float>());
		}
		return value;
	}

}