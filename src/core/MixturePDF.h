#ifndef MIXTUREPDF_H
#define MIXTUREPDF_H

#include "PDF.h"
#include "ArMathUtils.h"

namespace Aurora
{

	class MixturePDF : public PDF
	{
	private:
		PDF* m_pdf[2];

	public:
		MixturePDF(PDF *p0, PDF *p1) { m_pdf[0] = p0; m_pdf[1] = p1; }

		virtual Float value(const AVector3f &direction) const
		{
			return 0.5f * m_pdf[0]->value(direction) + 0.5f * m_pdf[1]->value(direction);
		}

		virtual AVector3f generate() const
		{
			if (drand48() < 0.5f)
				return m_pdf[0]->generate();
			else
				return m_pdf[1]->generate();
		}
	};

}

#endif // MIXTUREPDF_H
