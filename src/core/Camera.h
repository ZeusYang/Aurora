#ifndef CAMERA_H
#define CAMERA_H

#include "ArFilm.h"
#include "ArMathUtils.h"

namespace Aurora
{
	struct ACameraSample
	{
		AVector2f pFilm;
	};

	class Camera
	{
	public:
		typedef std::shared_ptr<Camera> ptr;

		AFilm::ptr m_film = nullptr;

		AVector3f m_pos;
		AVector3f m_target;
		AVector3f m_lowerLeftCorner;
		AVector3f m_horizontal;
		AVector3f m_vertical;
		Float m_fovy, m_aspect;
		Float m_lensRadius, m_focusDist;
		AVector3f m_axisX, m_axisY, m_axisZ;

		Camera(AFilm::ptr film, const AVector3f &cameraPos, const AVector3f &target, Float vfov,
			Float aspect, Float aperture, Float focus_dist);

		// Getter.
		ARay getRay(const Float &s, const Float &t) const;
		AVector3f getPosition() const { return m_pos; }
		AVector3f getTarget() const { return m_target; }
		AVector3f getAxisX() const { return m_axisX; }
		AVector3f getAxisY() const { return m_axisY; }
		AVector3f getAxisZ() const { return m_axisZ; }

		// Setter.
		void setPosition(const AVector3f &pos) { m_pos = pos; update(); }
		void setLenradius(const Float &radius) { m_lensRadius = radius; update(); }
		void setTarget(const AVector3f &_tar) { m_target = _tar; update(); }
		void setFovy(const Float &fov) { m_fovy = fov; update(); }
		void setAspect(const Float &asp) { m_aspect = asp; update(); }
		void setFoucusDist(const Float &fdist) { m_focusDist = fdist; update(); }

	private:
		AVector3f randomInUnitDisk() const;

		void update();
	};

}

#endif // CAMERA_H
