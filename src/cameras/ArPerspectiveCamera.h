#ifndef APERSPECTIVECAMERA_H
#define APERSPECTIVECAMERA_H

#include "ArCamera.h"

namespace Aurora
{

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