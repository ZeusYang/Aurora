#ifndef TEXTUREMGR_H
#define TEXTUREMGR_H

#include <vector>
#include "Texture.h"
#include "Singleton.h"

/**
 * @projectName   RayTracer
 * @brief         Texture manager.
 * @author        YangWC
 * @date          2019-05-15
 */

namespace RayTracer
{

	class TextureMgr : public Singleton<TextureMgr>
	{
	private:
		std::vector<Texture::ptr> m_units;

	public:
		typedef std::shared_ptr<TextureMgr> ptr;

		TextureMgr() = default;

		~TextureMgr() = default;

		static TextureMgr::ptr getSingleton();

		unsigned int loadTexture(Texture *target)
		{
			Texture::ptr newPtr(target);
			m_units.push_back(newPtr);
			return m_units.size() - 1;
		}

		Texture::ptr getTexture(unsigned int unit)
		{
			if (unit >= m_units.size())
				return nullptr;
			return m_units[unit];
		}

	};

}

#endif // TEXTUREMGR_H
