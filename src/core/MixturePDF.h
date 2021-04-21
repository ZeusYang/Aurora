#ifndef MIXTUREPDF_H
#define MIXTUREPDF_H

#include "PDF.h"

/**
 * @projectName   RayTracer
 * @brief         Mixture probalitity density function.
 * @author        YangWC
 * @date          2019-05-19
 */

namespace RayTracer
{

	class MixturePDF : public PDF
	{
	private:
		PDF* m_pdf[2];

	public:
		MixturePDF(PDF *p0, PDF *p1) { m_pdf[0] = p0; m_pdf[1] = p1; }

		virtual float value(const Vector3D &direction) const
		{
			return 0.5f * m_pdf[0]->value(direction) + 0.5f * m_pdf[1]->value(direction);
		}

		virtual Vector3D generate() const
		{
			if (drand48() < 0.5f)
				return m_pdf[0]->generate();
			else
				return m_pdf[1]->generate();
		}
	};

}

#endif // MIXTUREPDF_H
