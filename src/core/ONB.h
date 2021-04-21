#ifndef ONB_H
#define ONB_H

#include "ArMathUtils.h"

namespace Aurora
{

	class ONB
	{
	private:
		AVector3f m_axis[3];

	public:
		ONB() = default;
		AVector3f u() const { return m_axis[0]; }
		AVector3f v() const { return m_axis[1]; }
		AVector3f w() const { return m_axis[2]; }

		AVector3f operator[](int i) const { return m_axis[i]; }

		AVector3f local(Float a, Float b, Float c) const
		{
			return u() * a + v() * b + w() * c;
		}

		AVector3f local(const AVector3f &a) const
		{
			return u() * a.x + v() * a.y + w() * a.z;
		}

		void buildFromW(const AVector3f &n);

	};

}

#endif // ONB_H
