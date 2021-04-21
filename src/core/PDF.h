#ifndef PDF_H
#define PDF_H

#include "ArMathUtils.h"

namespace Aurora
{

	class PDF
	{
	public:
		virtual Float value(const AVector3f &driection) const = 0;
		virtual AVector3f generate() const = 0;
	};

}

#endif // PDF_H
