#ifndef HITABLEPDF_H
#define HITABLEPDF_H

#include "PDF.h"
#include "Hitable.h"

/**
 * @projectName   RayTracer
 * @brief         摘要
 * @author        YangWC
 * @date          2019-05-19
 */

namespace RayTracer
{

	class HitablePDF : public PDF
	{
	private:
		Hitable *m_target;
		Vector3D m_origin;

	public:
		HitablePDF(Hitable *p, const Vector3D &origin)
			:m_target(p), m_origin(origin) {}

		virtual float value(const Vector3D &direction) const
		{
			return m_target->pdfValue(m_origin, direction);
		}

		virtual Vector3D generate() const
		{
			return m_target->random(m_origin);
		}
	};

}

#endif // HITABLEPDF_H
