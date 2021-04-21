#include "CosinePDF.h"

namespace RayTracer
{

	float CosinePDF::value(const Vector3D &direction) const
	{
		Vector3D dir = direction;
		dir.normalize();
		float cosine = dir.dotProduct(uvw.w());
		if (cosine > 0.0f)
			return cosine / M_PI;
		else
			return 0.0f;
	}

	Vector3D CosinePDF::generate() const
	{
		return uvw.local(Vector3D::randomCosineDir());
	}


}
