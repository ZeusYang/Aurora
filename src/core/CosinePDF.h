#ifndef COSINEPDF_H
#define COSINEPDF_H

#include "PDF.h"
#include "ONB.h"

#include "ArMathUtils.h"

namespace Aurora
{

	class CosinePDF : public PDF
	{
	private:
		ONB uvw;

	public:
		CosinePDF(const AVector3f &w) { uvw.buildFromW(w); }

		virtual Float value(const AVector3f &driection) const;

		virtual AVector3f generate() const;

	};

}

#endif // COSINEPDF_H
