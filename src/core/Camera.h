#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include "Vector3D.h"

/**
 * @projectName   RayTracer
 * @brief         Camera class. (right-hand coordinate system)
 * @author        YangWC
 * @date          2019-05-06
 */

namespace RayTracer
{

	class Camera
	{
	public:
		Vector3D m_pos;
		Vector3D m_target;
		Vector3D m_lowerLeftCorner;
		Vector3D m_horizontal;
		Vector3D m_vertical;
		float m_fovy, m_aspect;
		float m_lensRadius, m_focusDist;
		Vector3D m_axisX, m_axisY, m_axisZ;

		Camera(const Vector3D &cameraPos, const Vector3D &target, float vfov,
			float aspect, float aperture, float focus_dist);

		// Getter.
		Ray getRay(const float &s, const float &t) const;
		Vector3D getPosition() const { return m_pos; }
		Vector3D getTarget() const { return m_target; }
		Vector3D getAxisX() const { return m_axisX; }
		Vector3D getAxisY() const { return m_axisY; }
		Vector3D getAxisZ() const { return m_axisZ; }

		// Setter.
		void setPosition(const Vector3D &pos) { m_pos = pos; update(); }
		void setLenradius(const float &radius) { m_lensRadius = radius; update(); }
		void setTarget(const Vector3D &_tar) { m_target = _tar; update(); }
		void setFovy(const float &fov) { m_fovy = fov; update(); }
		void setAspect(const float &asp) { m_aspect = asp; update(); }
		void setFoucusDist(const float &fdist) { m_focusDist = fdist; update(); }

	private:
		Vector3D randomInUnitDisk() const;

		void update();
	};

}

#endif // CAMERA_H
