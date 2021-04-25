#ifndef ARTRANSFORM_H
#define ARTRANSFORM_H

#include "ArAurora.h"
#include "ArMathUtils.h"

namespace Aurora
{

	class ATransform
	{
	public:
		typedef std::shared_ptr<ATransform> ptr;

		ATransform() {}

		ATransform(const Float mat[4][4])
		{
			//Note: column major matrix
			m_trans = AMatrix4x4(
				mat[0][0], mat[1][0], mat[2][0], mat[3][0],
				mat[0][1], mat[1][1], mat[2][1], mat[3][1],
				mat[0][2], mat[1][2], mat[2][2], mat[3][2],
				mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
			m_transInv = inverse(m_trans);
		}

		ATransform(const AMatrix4x4 &m) : m_trans(m), m_transInv(inverse(m)) {}
		ATransform(const AMatrix4x4 &m, const AMatrix4x4 &mInv) : m_trans(m), m_transInv(mInv) {}

		friend ATransform inverse(const ATransform &t) { return ATransform(t.m_transInv, t.m_trans); }
		friend ATransform transpose(const ATransform &t) { return ATransform(transpose(t.m_trans), transpose(t.m_trans)); }

		bool operator==(const ATransform &t) const { return t.m_trans == m_trans && t.m_transInv == m_transInv; }
		bool operator!=(const ATransform &t) const { return t.m_trans != m_trans || t.m_transInv != m_transInv; }

		//Ray
		inline ARay operator()(const ARay &r) const;
		//Bounds
		ABounds3f operator()(const ABounds3f &b) const;
		//SurfaceInteraction
		ASurfaceInteraction operator()(const ASurfaceInteraction &si) const;
		//Vector
		template <typename T>
		inline AVector3<T> operator()(const AVector3<T> &p, const Float &w) const;

		bool isIdentity() const
		{
			return (
				m_trans[0][0] == 1.f && m_trans[1][0] == 0.f && m_trans[2][0] == 0.f && m_trans[3][0] == 0.f &&
				m_trans[0][1] == 0.f && m_trans[1][1] == 1.f && m_trans[2][1] == 0.f && m_trans[3][1] == 0.f &&
				m_trans[0][2] == 0.f && m_trans[1][2] == 0.f && m_trans[2][2] == 1.f && m_trans[3][2] == 0.f &&
				m_trans[0][3] == 0.f && m_trans[1][3] == 0.f && m_trans[2][3] == 0.f && m_trans[3][3] == 1.f);
		}

		const AMatrix4x4 &getMatrix() const { return m_trans; }
		const AMatrix4x4 &getInverseMatrix() const { return m_transInv; }

		ATransform operator*(const ATransform &t2) const;

	private:
		AMatrix4x4 m_trans, m_transInv;
	};

	ATransform translate(const AVector3f &delta);
	ATransform scale(Float x, Float y, Float z);
	ATransform rotateX(Float theta);
	ATransform rotateY(Float theta);
	ATransform rotateZ(Float theta);
	ATransform rotate(Float theta, const AVector3f &axis);
	ATransform lookAt(const AVector3f &pos, const AVector3f &look, const AVector3f &up);
	ATransform orthographic(Float znear, Float zfar);
	ATransform perspective(Float fov, Float znear, Float zfar);

	template <typename T>
	inline AVector3<T> ATransform::operator()(const AVector3<T> &p, const Float &w) const
	{
		//Note: w == 1.f -> point, w == 0.f -> vector
		glm::vec<4, Float> ret = m_trans * glm::vec<4, Float>(p.x, p.y, p.z, w);
		if (w == 0.f)
			return AVector3<T>(ret.x, ret.y, ret.z);

		CHECK_NE(ret.w, 0);
		if (ret.w == 1)
			return AVector3<T>(ret.x, ret.y, ret.z);
		else
			return AVector3<T>(ret.x, ret.y, ret.z) / ret.w;
	}

	inline ARay ATransform::operator()(const ARay &r) const
	{
		AVector3f o = (*this)(r.m_origin, 1.0f);
		AVector3f d = (*this)(r.m_dir, 0.0f);
		Float tMax = r.m_tMax;

		return ARay(o, d, tMax);
	}

}

#endif