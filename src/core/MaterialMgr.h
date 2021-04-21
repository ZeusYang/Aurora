#ifndef MATERIALMGR_H
#define MATERIALMGR_H

#include <vector>
#include "Singleton.h"
#include "Material.h"

/**
 * @projectName   RayTracer
 * @brief         Material manager.
 * @author        YangWC
 * @date          2019-05-15
 */

namespace RayTracer
{

	class MaterialMgr : public Singleton<MaterialMgr>
	{
	private:
		std::vector<Material::ptr> m_units;

	public:
		typedef std::shared_ptr<MaterialMgr> ptr;

		MaterialMgr() = default;

		~MaterialMgr() = default;

		static MaterialMgr::ptr getSingleton();

		unsigned int loadMaterial(Material *target)
		{
			Material::ptr newPtr(target);
			m_units.push_back(newPtr);
			return m_units.size() - 1;
		}

		Material::ptr getMaterial(unsigned int unit)
		{
			if (unit >= m_units.size())
				return nullptr;
			return m_units[unit];
		}
	};

}

#endif // MATERIALMGR_H
