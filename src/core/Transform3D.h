#ifndef TRANSFORM3D_H
#define TRANSFORM3D_H

#include "ArMathUtils.h"

namespace Aurora
{

	class Transform3D
	{
	private:
		mutable bool m_dirty;       // Should update or not.
		AVector3f m_scale;           // Object's scale.
		AQuaterion m_rotation;      // Object's rotation.
		AVector3f m_translation;     // Object's translation.
		AMatrix4x4 m_world;          // Object's model matrix.
		AMatrix4x4 m_invWorld;       // Object's normal matrix;

	public:
		// Object's local axis.
		static const AVector3f LocalForward;
		static const AVector3f LocalUp;
		static const AVector3f LocalRight;

		// ctor/dtor
		Transform3D();
		~Transform3D() = default;

		// Getter.
		AMatrix4x4 toMatrix();
		AMatrix4x4 toInvMatrix();

		// Transformation.
		void scale(const AVector3f &ds);
		void translate(const AVector3f &dt);
		void rotate(const AVector3f &axis, Float angle);
		void setScale(const AVector3f &s);
		void setRotation(const AQuaterion &r);
		void setTranslation(const AVector3f &t);

		// Query object's axis.
		AVector3f forward() const;
		AVector3f up() const;
		AVector3f right() const;

		// Transformation getter.
		AVector3f translation() const { return m_translation; }
		AQuaterion rotation() const { return m_rotation; }
		AVector3f scale() const { return m_scale; }
		bool getDirtry() const { return m_dirty; }
	};

}

#endif // TRANSFORM3D_H
