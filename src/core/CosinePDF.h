#ifndef COSINEPDF_H
#define COSINEPDF_H

#include "PDF.h"
#include "ONB.h"

/**
 * @projectName   RayTracer
 * @brief         摘要
 * @author        YangWC
 * @date          2019-05-18
 */

namespace RayTracer
{

	class CosinePDF : public PDF
	{
	private:
		ONB uvw;

	public:
		CosinePDF(const Vector3D &w) { uvw.buildFromW(w); }

		virtual float value(const Vector3D &driection) const;

		virtual Vector3D generate() const;

	};

}

#endif // COSINEPDF_H
