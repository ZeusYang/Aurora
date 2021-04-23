#ifndef PDF_H
#define PDF_H

#include "ArMathUtils.h"

#include "ONB.h"
#include "Hitable.h"

namespace Aurora
{

	class PDF
	{
	public:
		virtual Float value(const AVector3f &driection) const = 0;
		virtual AVector3f generate() const = 0;
	};

	class CosinePDF : public PDF
	{
	private:
		ONB uvw;

	public:
		CosinePDF(const AVector3f &w) { uvw.buildFromW(w); }

		virtual Float value(const AVector3f &driection) const;

		virtual AVector3f generate() const;

	};

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

#endif // PDF_H
