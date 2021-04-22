#include "Camera.h"

namespace Aurora
{
	Camera::Camera(AFilm::ptr film, const AVector3f &cameraPos, const AVector3f &target,
		Float vfov, Float aspect, Float aperture, Float focus_dist)
		: m_film(film)
	{
		m_pos = cameraPos;
		m_target = target;
		m_fovy = vfov;
		m_aspect = aspect;
		m_lensRadius = aperture * 0.5f;
		m_focusDist = focus_dist;
		update();
	}

	ARay Camera::getRay(const Float &s, const Float &t) const
	{
		AVector3f rd = randomInUnitDisk() * m_lensRadius;
		AVector3f offset = m_axisX * rd.x + m_axisY * rd.y;
		return ARay(m_pos + offset, normalize(m_lowerLeftCorner + m_horizontal * s
			+ m_vertical * t - m_pos - offset));
	}

	AVector3f Camera::randomInUnitDisk() const
	{
		AVector3f p;
		do {
			p = AVector3f(drand48(), drand48(), 0) * 2.0f - AVector3f(1.0f, 1.0f, 0.0f);
		} while (dot(p, p) >= 1.0f);
		return p;
	}

	void Camera::update()
	{
		const AVector3f worldUp(0.0f, 1.0f, 0.0f);
		// frustum.
		Float theta = radians(m_fovy);
		Float half_height = static_cast<Float>(tan(theta * 0.5f)) * m_focusDist;
		Float half_width = m_aspect * half_height;

		// camera coordinate system.
		m_axisZ = m_pos - m_target;
		m_axisZ = normalize(m_axisZ);
		m_axisX = cross(worldUp, m_axisZ);
		m_axisX = normalize(m_axisX);
		m_axisY = cross(m_axisZ, m_axisX);
		m_axisY = normalize(m_axisY);

		// view port.
		m_lowerLeftCorner = m_pos - m_axisX * half_width
			- m_axisY * half_height - m_axisZ * m_focusDist;
		m_horizontal = m_axisX * 2.0f * half_width;
		m_vertical = m_axisY * 2.0f * half_height;
	}
}
