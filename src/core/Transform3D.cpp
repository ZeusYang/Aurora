#include "Transform3D.h"

namespace Aurora
{

	const AVector3f Transform3D::LocalUp(0.0f, 1.0f, 0.0f);
	const AVector3f Transform3D::LocalForward(0.0f, 0.0f, 1.0f);
	const AVector3f Transform3D::LocalRight(1.0f, 0.0f, 0.0f);

	Transform3D::Transform3D()
		:m_dirty(true), m_scale(1.0, 1.0, 1.0)
	{
		m_world = glm::mat4(1.0f);
		m_invWorld = glm::mat4(1.0f);
		m_rotation = AQuaterion(1.0f, 0.0f, 0.0f, 0.0f);
	}

	AMatrix4x4 Transform3D::toMatrix()
	{
		if (m_dirty)
		{
			m_dirty = false;
			//m_world = m_rotation.toMatrix();
			m_world = toMatrix4x4(m_rotation);
			AMatrix4x4 trans, scals;
			scals = glm::scale(glm::mat4(1.0f), m_scale);
			//scals.setScale(m_scale);
			//trans.setTranslation(m_translation);
			trans = glm::translate(glm::mat4(1.0f), m_translation);
			m_world = trans * m_world * scals;
			//m_invWorld = m_world.getInverseTranspose();
			m_invWorld = inverse(transpose(m_world));
		}
		return m_world;
	}

	AMatrix4x4 Transform3D::toInvMatrix()
	{
		if (m_dirty)
		{
			m_dirty = false;
			m_world = toMatrix4x4(m_rotation);
			//m_world = m_rotation.toMatrix();
			AMatrix4x4 trans, scals;
			//scals.setScale(m_scale);
			scals = glm::scale(glm::mat4(1.0f), m_scale);
			//trans.setTranslation(m_translation);
			trans = glm::translate(glm::mat4(1.0f), m_translation);
			m_world = trans * m_world * scals;
			//m_invWorld = m_world.getInverseTranspose();
			m_invWorld = inverse(transpose(m_world));
		}
		return m_invWorld;
	}

	void Transform3D::scale(const AVector3f &ds)
	{
		m_dirty = true;
		m_scale.x *= ds.x;
		m_scale.y *= ds.y;
		m_scale.z *= ds.z;
	}

	void Transform3D::translate(const AVector3f &dt)
	{
		m_dirty = true;
		m_translation += dt;
	}

	void Transform3D::rotate(const AVector3f &axis, Float angle)
	{
		m_dirty = true;
		Float ang = glm::radians(angle);
		AQuaterion newRot = AQuaterion(
			glm::cos(ang / 2),
			axis.x * glm::sin(ang / 2),
			axis.y * glm::sin(ang / 2),
			axis.z * glm::sin(ang / 2));

		//newRot.setRotationAxis(axis, angle);
		//m_rotation = m_rotation * newRot;
		m_rotation = newRot * m_rotation;
	}

	void Transform3D::setScale(const AVector3f &s)
	{
		m_dirty = true;
		m_scale = s;
	}

	void Transform3D::setRotation(const AQuaterion &r)
	{
		m_dirty = true;
		m_rotation = r;
	}

	void Transform3D::setTranslation(const AVector3f &t)
	{
		m_dirty = true;
		m_translation = t;
	}

	AVector3f Transform3D::forward() const
	{
		return m_rotation * LocalForward;
	}

	AVector3f Transform3D::up() const
	{
		return m_rotation * LocalUp;
	}

	AVector3f Transform3D::right() const
	{
		return m_rotation * LocalRight;
	}


}
