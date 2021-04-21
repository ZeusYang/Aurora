#ifndef PDF_H
#define PDF_H

#include "Vector3D.h"

/**
 * @projectName   RayTracer
 * @brief         Posibility Density Function.
 * @author        YangWC
 * @date          2019-05-18
 */

namespace RayTracer
{

	class PDF
	{
	public:
		virtual float value(const Vector3D &driection) const = 0;
		virtual Vector3D generate() const = 0;
	};

}

#endif // PDF_H
