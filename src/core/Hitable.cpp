#include "Hitable.h"

#include "ONB.h"
#include "ArInteraction.h"

namespace Aurora
{

	bool HitableEntity::hit(const ARay &ray, HitRecord &ret) const
	{
		Float tHit;
		ASurfaceInteraction isect;
		if (!m_shape->hit(ray, tHit, isect))
			return false;

		ray.m_tMax = tHit;
		ret.m_material = m_material.get();
		ret.m_position = isect.p;
		ret.m_normal = isect.n;
		ret.m_texcoord = isect.uv;
		return true;
	}

	bool HitableList::hit(const ARay &ray, HitRecord &ret) const
	{
		HitRecord temp_rec;
		bool hit_anything = false;
		for (int i = 0; i < m_objects.size(); i++)
		{
			if (m_objects[i]->hit(ray, temp_rec))
			{
				hit_anything = true;
				ret = temp_rec;
			}
		}
		return hit_anything;
	}

	Float HitableList::pdfValue(const AVector3f &o, const AVector3f &v) const
	{
		Float weight = 1.0f / m_objects.size();
		Float sum = 0;
		for (int x = 0; x < m_objects.size(); ++x)
			sum += m_objects[x]->pdfValue(o, v);
		return sum * weight;
	}

	AVector3f HitableList::random(const AVector3f &o) const
	{
		int index = static_cast<int>(drand48() * m_objects.size());
		return m_objects[index]->random(o);
	}

}
