#ifndef ACAMERA_H
#define ACAMERA_H

#include "ArAurora.h"
#include "ArFilm.h"
#include "ArTransform.h"
#include "ArRtti.h"

namespace Aurora
{
	struct ACameraSample
	{
		AVector2f pFilm;
	};

	inline std::ostream &operator<<(std::ostream &os, const ACameraSample &cs) 
	{
		os << "[ pFilm: " << cs.pFilm << " ]";
		return os;
	}

	class ACamera : public AObject
	{
	public:
		typedef std::shared_ptr<ACamera> ptr;

		// Camera Interface
		ACamera() = default;
		ACamera(const ATransform &cameraToWorld, AFilm::ptr film);
		virtual ~ACamera();

		virtual Float castingRay(const ACameraSample &sample, ARay &ray) const = 0;

		//virtual spectrum We(const ARay &ray, APoint2f *pRaster2 = nullptr) const;
		//virtual void pdf_We(const ARay &ray, Float *pdfPos, Float *pdfDir) const;

		//virtual Spectrum Sample_Wi(const Interaction &ref, const APoint2f &u,
		//	AVector3f *wi, Float *pdf, APoint2f *pRaster, VisibilityTester *vis) const;

		virtual AClassType getClassType() const override { return AClassType::AECamera; }

		// Camera Public Data
		ATransform m_cameraToWorld;
		AFilm::ptr m_film = nullptr;
	};

	class AProjectiveCamera : public ACamera
	{
	public:
		typedef std::shared_ptr<AProjectiveCamera> ptr;

		AProjectiveCamera() = default;
		AProjectiveCamera(const ATransform &cameraToWorld, const ATransform &cameraToScreen, AFilm::ptr film)
			: ACamera(cameraToWorld, film), m_cameraToScreen(cameraToScreen) { }

	protected:
		virtual void initialize();

	protected:
		ATransform m_cameraToScreen, m_rasterToCamera;
		ATransform m_screenToRaster, m_rasterToScreen;
	};

	class APerspectiveCamera final : public AProjectiveCamera
	{
	public:
		typedef std::shared_ptr<APerspectiveCamera> ptr;

		APerspectiveCamera(const APropertyTreeNode &node);
		APerspectiveCamera(const ATransform &CameraToWorld, Float fov, AFilm::ptr film);

		virtual Float castingRay(const ACameraSample &sample, ARay &ray) const override;

		//Spectrum We(const Ray &ray, Point2f *pRaster2 = nullptr) const;
		//void Pdf_We(const Ray &ray, Float *pdfPos, Float *pdfDir) const;
		//Spectrum Sample_Wi(const Interaction &ref, const Point2f &sample,
		//	Vector3f *wi, Float *pdf, Point2f *pRaster,
		//	VisibilityTester *vis) const;

		virtual void activate() override { initialize(); }

		virtual std::string toString() const override { return "PerspectiveCamera[]"; }

	protected:
		virtual void initialize() override;

	private:
		//AVector3f dxCamera, dyCamera;
		Float A;
	};

}

#endif