#ifndef HITABLEPDF_H
#define HITABLEPDF_H

#include "PDF.h"
#include "Hitable.h"


namespace Aurora
{

	class HitablePDF : public PDF
	{
	private:
		Hitable *m_target;
		AVector3f m_origin;

	public:
		HitablePDF(Hitable *p, const AVector3f &origin)
			:m_target(p), m_origin(origin) {}

		virtual Float value(const AVector3f &direction) const
		{
			return m_target->pdfValue(m_origin, direction);
		}

		virtual AVector3f generate() const
		{
			return m_target->random(m_origin);
		}
	};

}

#endif // HITABLEPDF_H
