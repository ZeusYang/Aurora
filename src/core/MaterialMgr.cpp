#include "MaterialMgr.h"

namespace RayTracer
{
	template<> MaterialMgr::ptr Singleton<MaterialMgr>::_instance = nullptr;

	MaterialMgr::ptr MaterialMgr::getSingleton()
	{
		if (_instance == nullptr)
			return _instance = std::make_shared<MaterialMgr>();
		return _instance;
	}
}
