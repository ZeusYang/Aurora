#include "TextureMgr.h"

namespace Aurora
{
	template<> TextureMgr::ptr Singleton<TextureMgr>::_instance = nullptr;

	TextureMgr::ptr TextureMgr::getSingleton()
	{
		if (_instance == nullptr)
			return _instance = std::make_shared<TextureMgr>();
		return _instance;
	}
}