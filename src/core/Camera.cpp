#include "Camera.h"

namespace RayTracer
{
	Camera::Camera(const Vector3D &cameraPos, const Vector3D &target,
		float vfov, float aspect, float aperture, float focus_dist)
	{
		m_pos = cameraPos;
		m_target = target;
		m_fovy = vfov;
		m_aspect = aspect;
		m_lensRadius = aperture * 0.5f;
		m_focusDist = focus_dist;
		update();
	}

	Ray Camera::getRay(const float &s, const float &t) const
	{
		Vector3D rd = randomInUnitDisk() * m_lensRadius;
		Vector3D offset = m_axisX * rd.x + m_axisY * rd.y;
		return Ray(m_pos + offset, m_lowerLeftCorner + m_horizontal * s
			+ m_vertical * t - m_pos - offset);
	}

	Vector3D Camera::randomInUnitDisk() const
	{
		Vector3D p;
		do {
			p = Vector3D(drand48(), drand48(), 0) * 2.0f - Vector3D(1.0f, 1.0f, 0.0f);
		} while (p.dotProduct(p) >= 1.0f);
		return p;
	}

	void Camera::update()
	{
		const Vector3D worldUp(0.0f, 1.0f, 0.0f);
		// frustum.
		float theta = radians(m_fovy);
		float half_height = static_cast<float>(tan(theta * 0.5f)) * m_focusDist;
		float half_width = m_aspect * half_height;

		// camera coordinate system.
		m_axisZ = m_pos - m_target;
		m_axisZ.normalize();
		m_axisX = worldUp.crossProduct(m_axisZ);
		m_axisX.normalize();
		m_axisY = m_axisZ.crossProduct(m_axisX);
		m_axisY.normalize();

		// view port.
		m_lowerLeftCorner = m_pos - m_axisX * half_width
			- m_axisY * half_height - m_axisZ * m_focusDist;
		m_horizontal = m_axisX * 2.0f * half_width;
		m_vertical = m_axisY * 2.0f * half_height;
	}
}
