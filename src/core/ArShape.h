#ifndef ARSHAPE_H
#define ARSHAPE_H

#include "ArAurora.h"
#include "ArMathUtils.h"
#include "ArTransform.h"
#include "ArRtti.h"

#include <vector>

namespace Aurora
{
	class AShape : public AObject
	{
	public:
		typedef std::shared_ptr<AShape> ptr;

		AShape(const APropertyList &props);
		AShape(const ATransform &objectToWorld, const ATransform &worldToObject);
		virtual ~AShape() = default;

		virtual ABounds3f objectBound() const = 0;
		virtual ABounds3f worldBound() const;

		virtual bool hit(const ARay &ray) const;
		virtual bool hit(const ARay &ray, Float &tHit, ASurfaceInteraction &isect) const = 0;

		virtual Float area() const = 0;

		// Sample a point on the surface of the shape and return the PDF with
		// respect to area on the surface.
		virtual AInteraction sample(const AVector2f &u, Float &pdf) const = 0;
		virtual Float pdf(const AInteraction &) const { return 1 / area(); }

		// Sample a point on the shape given a reference point |ref| and
		// return the PDF with respect to solid angle from |ref|.
		virtual AInteraction sample(const AInteraction &ref, const AVector2f &u, Float &pdf) const;
		virtual Float pdf(const AInteraction &ref, const AVector3f &wi) const;

		// Returns the solid angle subtended by the shape w.r.t. the reference
		// point p, given in world space. Some shapes compute this value in
		// closed-form, while the default implementation uses Monte Carlo
		// integration; the nSamples parameter determines how many samples are
		// used in this case.
		virtual Float solidAngle(const AVector3f &p, int nSamples = 512) const;

		virtual AClassType getClassType() const override { return AClassType::AEShape; }

		ATransform m_objectToWorld, m_worldToObject;
	};

	class ASphereShape final : public AShape
	{
	public:
		typedef std::shared_ptr<ASphereShape> ptr;

		ASphereShape(const APropertyTreeNode &node);
		ASphereShape(const ATransform &objectToWorld, const ATransform &worldToObject, const float radius);

		virtual ~ASphereShape() = default;

		virtual Float area() const override;

		virtual AInteraction sample(const AVector2f &u, Float &pdf) const override;

		virtual AInteraction sample(const AInteraction &ref, const AVector2f &u, Float &pdf) const override;
		virtual Float pdf(const AInteraction &ref, const AVector3f &wi) const override;

		virtual ABounds3f objectBound() const override;

		virtual bool hit(const ARay &ray) const override;
		virtual bool hit(const ARay &ray, Float &tHit, ASurfaceInteraction &isect) const override;

		virtual Float solidAngle(const AVector3f &p, int nSamples = 512) const override;

		virtual std::string toString() const override { return "SphereShape[]"; }

	private:
		float m_radius;
	};

	class ATriangleMesh final
	{
	public:
		typedef std::shared_ptr<ATriangleMesh> ptr;

		ATriangleMesh(const ATransform &objectToWorld, const std::string &filename);

		size_t numTriangles() const { return m_indices.size() / 3; }
		size_t numVertices() const { return m_nVertices; }

		const AVector3f& getPosition(const int &index) const { return m_position[index]; }
		const AVector3f& getNormal(const int &index) const { return m_normal[index]; }
		const AVector2f& getUV(const int &index) const { return m_uv[index]; }

	private:

		// TriangleMesh Data
		std::unique_ptr<AVector3f[]> m_position = nullptr;
		std::unique_ptr<AVector3f[]> m_normal = nullptr;
		std::unique_ptr<AVector2f[]> m_uv = nullptr;
		std::vector<int> m_indices;
		int m_nVertices;
	};

	class ATriangleShape final : public AShape
	{
	public:
		typedef std::shared_ptr<ATriangleShape> ptr;

		ATriangleShape(const APropertyTreeNode &node);
		ATriangleShape(const ATransform &objectToWorld, const ATransform &worldToObject, AVector3f v[3]);

		virtual ~ATriangleShape() = default;

		virtual Float area() const override;

		virtual AInteraction sample(const AVector2f &u, Float &pdf) const override;

		virtual ABounds3f objectBound() const override;
		virtual ABounds3f worldBound() const override;

		virtual bool hit(const ARay &ray) const override;
		virtual bool hit(const ARay &ray, Float &tHit, ASurfaceInteraction &isect) const override;

		virtual Float solidAngle(const AVector3f &p, int nSamples = 512) const override;

		virtual std::string toString() const override { return "TriangleShape[]"; }

	private:
		AVector3f m_p0, m_p1, m_p2;
	};

}

#endif