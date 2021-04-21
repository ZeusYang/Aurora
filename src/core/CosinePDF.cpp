#include "CosinePDF.h"

namespace Aurora
{

	Float CosinePDF::value(const AVector3f &direction) const
	{
		AVector3f dir = normalize(direction);
		Float cosine = dot(dir, uvw.w());
		if (cosine > 0.0f)
			return cosine / aPi;
		else
			return 0.0f;
	}

	AVector3f CosinePDF::generate() const
	{
		return uvw.local(randomCosineDir());
	}


}
