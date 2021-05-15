#ifndef ARTRIANGLE_SHAPE_H
#define ARTRIANGLE_SHAPE_H

#include "ArShape.h"

namespace Aurora
{
	class ATriangleMesh final
	{
	public:
		typedef std::shared_ptr<ATriangleMesh> ptr;
		typedef std::unique_ptr<ATriangleMesh> unique_ptr;

		ATriangleMesh(ATransform *objectToWorld, const std::string &filename);

		size_t numTriangles() const { return m_indices.size() / 3; }
		size_t numVertices() const { return m_nVertices; }

		bool hasUV() const { return m_uv != nullptr; }
		bool hasNormal() const { return m_normal != nullptr; }

		const AVector3f& getPosition(const int &index) const { return m_position[index]; }
		const AVector3f& getNormal(const int &index) const { return m_normal[index]; }
		const AVector2f& getUV(const int &index) const { return m_uv[index]; }

		const std::vector<int>& getIndices() const { return m_indices; }

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
		ATriangleShape(ATransform *objectToWorld, ATransform *worldToObject,
			std::array<int, 3> indices, ATriangleMesh *mesh);

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
		ATriangleMesh *m_mesh;
		std::array<int, 3> m_indices;
	};
}

#endif