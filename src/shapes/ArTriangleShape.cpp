#include "ArTriangleShape.h"

#include <array>

#include "ArSampler.h"
#include "ArInteraction.h"

#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

namespace Aurora
{
	//-------------------------------------------ATriangleMesh-------------------------------------

	ATriangleMesh::ATriangleMesh(ATransform *objectToWorld, const std::string &filename)
	{
		std::vector<AVector3f> gPosition;
		std::vector<AVector3f> gNormal;
		std::vector<AVector2f> gUV;
		std::vector<int> gIndices;

		auto process_mesh = [&](aiMesh *mesh, const aiScene *scene) -> void
		{
			// Walk through each of the mesh's vertices
			std::vector<AVector3f> position;
			std::vector<AVector3f> normal;
			std::vector<AVector2f> uv;
			std::vector<int> indices;
			for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
			{
				position.push_back(AVector3f(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
				normal.push_back(AVector3f(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
				if (mesh->mTextureCoords[0])
				{
					uv.push_back(AVector2f(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
				}
			}

			for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
			{
				aiFace face = mesh->mFaces[i];
				// Retrieve all indices of the face and store them in the indices vector
				for (unsigned int j = 0; j < face.mNumIndices; ++j)
				{
					indices.push_back(face.mIndices[j] + gPosition.size());
				}
			}

			// Merge to one mesh
			gPosition.insert(gPosition.end(), position.begin(), position.end());
			gNormal.insert(gNormal.end(), normal.begin(), normal.end());
			gUV.insert(gUV.end(), uv.begin(), uv.end());
			gIndices.insert(gIndices.end(), indices.begin(), indices.end());
		};

		std::function<void(aiNode *node, const aiScene *scene)> process_node;
		process_node = [&](aiNode *node, const aiScene *scene) -> void
		{
			// Process each mesh located at the current node
			for (unsigned int i = 0; i < node->mNumMeshes; ++i)
			{
				// The node object only contains indices to index the actual objects in the scene. 
				// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				process_mesh(mesh, scene);
			}

			// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				process_node(node->mChildren[i], scene);
			}
		};
		// Import the mesh using ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals
			| aiProcess_FlipUVs | aiProcess_FixInfacingNormals | aiProcess_OptimizeMeshes);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			LOG(FATAL) << "ERROR::ASSIMP:: " << importer.GetErrorString();
		}

		// Process the mesh node
		process_node(scene->mRootNode, scene);

		// Vertex data
		// Note: we transform the vertex into world space in advance for efficient ray intersection routine
		m_nVertices = gPosition.size();
		m_position.reset(new AVector3f[m_nVertices]);
		if (!gNormal.empty())
		{
			m_normal.reset(new AVector3f[m_nVertices]);
		}
		if (!gUV.empty())
		{
			m_uv.reset(new AVector2f[m_nVertices]);
		}

		for (unsigned int i = 0; i < m_nVertices; ++i)
		{
			m_position[i] = (*objectToWorld)(gPosition[i], 1.0f);
			if (m_normal != nullptr)
			{
				m_normal[i] = (*objectToWorld)(gNormal[i], 0.0f);
			}
			if (m_uv != nullptr)
			{
				m_uv[i] = gUV[i];
			}
		}

		m_indices.resize(gIndices.size());
		m_indices.assign(gIndices.begin(), gIndices.end());

	}

	//-------------------------------------------ATriangleShape-------------------------------------

	AURORA_REGISTER_CLASS(ATriangleShape, "Triangle")

	ATriangleShape::ATriangleShape(const APropertyTreeNode &node)
		:AShape(node.getPropertyList())
	{
		//const auto &props = node.getPropertyList();
		//m_p0 = (props.getVector3f("P0"));
		//m_p1 = (props.getVector3f("P1"));
		//m_p2 = (props.getVector3f("P2"));
		//activate();
	}

	ATriangleShape::ATriangleShape(ATransform *objectToWorld, ATransform *worldToObject,
		std::array<int, 3> indices, ATriangleMesh *mesh) : AShape(objectToWorld, worldToObject), m_indices(indices), m_mesh(mesh) {}

	ABounds3f ATriangleShape::objectBound() const
	{
		// Get triangle vertices in _p0_, _p1_, and _p2_
		const auto &p0 = m_mesh->getPosition(m_indices[0]);
		const auto &p1 = m_mesh->getPosition(m_indices[1]);
		const auto &p2 = m_mesh->getPosition(m_indices[2]);
		return unionBounds(ABounds3f((*m_worldToObject)(p0, 1.0f), (*m_worldToObject)(p1, 1.0f)), (*m_worldToObject)(p2, 1.0f));
	}

	ABounds3f ATriangleShape::worldBound() const
	{
		// Get triangle vertices in _p0_, _p1_, and _p2_
		const auto &p0 = m_mesh->getPosition(m_indices[0]);
		const auto &p1 = m_mesh->getPosition(m_indices[1]);
		const auto &p2 = m_mesh->getPosition(m_indices[2]);
		return unionBounds(ABounds3f(p0, p1), p2);
	}

	Float ATriangleShape::area() const
	{
		// Get triangle vertices in _p0_, _p1_, and _p2_
		const auto &p0 = m_mesh->getPosition(m_indices[0]);
		const auto &p1 = m_mesh->getPosition(m_indices[1]);
		const auto &p2 = m_mesh->getPosition(m_indices[2]);
		return 0.5 * length(cross(p1 - p0, p2 - p0));
	}

	AInteraction ATriangleShape::sample(const AVector2f &u, Float &pdf) const
	{
		AVector2f b = uniformSampleTriangle(u);
		// Get triangle vertices in _p0_, _p1_, and _p2_
		const auto &p0 = m_mesh->getPosition(m_indices[0]);
		const auto &p1 = m_mesh->getPosition(m_indices[1]);
		const auto &p2 = m_mesh->getPosition(m_indices[2]);
		AInteraction it;
		it.p = b[0] * p0 + b[1] * p1 + (1 - b[0] - b[1]) * p2;
		// Compute surface normal for sampled point on triangle
		it.n = normalize(AVector3f(cross(p1 - p0, p2 - p0)));

		pdf = 1 / area();
		return it;
	}

	bool ATriangleShape::hit(const ARay &ray) const
	{
		// Get triangle vertices in _p0_, _p1_, and _p2_
		const auto &p0 = m_mesh->getPosition(m_indices[0]);
		const auto &p1 = m_mesh->getPosition(m_indices[1]);
		const auto &p2 = m_mesh->getPosition(m_indices[2]);

		// Perform ray--triangle intersection test

		// Transform triangle vertices to ray coordinate space

		// Translate vertices based on ray origin
		AVector3f p0t = p0 - AVector3f(ray.origin());
		AVector3f p1t = p1 - AVector3f(ray.origin());
		AVector3f p2t = p2 - AVector3f(ray.origin());

		// Permute components of triangle vertices and ray direction
		int kz = maxDimension(abs(ray.direction()));
		int kx = kz + 1;
		if (kx == 3) kx = 0;
		int ky = kx + 1;
		if (ky == 3) ky = 0;
		AVector3f d = permute(ray.direction(), kx, ky, kz);
		p0t = permute(p0t, kx, ky, kz);
		p1t = permute(p1t, kx, ky, kz);
		p2t = permute(p2t, kx, ky, kz);

		// Apply shear transformation to translated vertex positions
		Float Sx = -d.x / d.z;
		Float Sy = -d.y / d.z;
		Float Sz = 1.f / d.z;
		p0t.x += Sx * p0t.z;
		p0t.y += Sy * p0t.z;
		p1t.x += Sx * p1t.z;
		p1t.y += Sy * p1t.z;
		p2t.x += Sx * p2t.z;
		p2t.y += Sy * p2t.z;

		// Compute edge function coefficients _e0_, _e1_, and _e2_
		Float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
		Float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
		Float e2 = p0t.x * p1t.y - p0t.y * p1t.x;

		// Fall back to double precision test at triangle edges
		if (sizeof(Float) == sizeof(float) &&
			(e0 == 0.0f || e1 == 0.0f || e2 == 0.0f))
		{
			double p2txp1ty = (double)p2t.x * (double)p1t.y;
			double p2typ1tx = (double)p2t.y * (double)p1t.x;
			e0 = (float)(p2typ1tx - p2txp1ty);
			double p0txp2ty = (double)p0t.x * (double)p2t.y;
			double p0typ2tx = (double)p0t.y * (double)p2t.x;
			e1 = (float)(p0typ2tx - p0txp2ty);
			double p1txp0ty = (double)p1t.x * (double)p0t.y;
			double p1typ0tx = (double)p1t.y * (double)p0t.x;
			e2 = (float)(p1typ0tx - p1txp0ty);
		}

		// Perform triangle edge and determinant tests
		if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
			return false;
		Float det = e0 + e1 + e2;
		if (det == 0)
			return false;

		// Compute scaled hit distance to triangle and test against ray $t$ range
		p0t.z *= Sz;
		p1t.z *= Sz;
		p2t.z *= Sz;
		Float tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
		if (det < 0 && (tScaled >= 0 || tScaled < ray.m_tMax * det))
			return false;
		else if (det > 0 && (tScaled <= 0 || tScaled > ray.m_tMax * det))
			return false;

		// Compute barycentric coordinates and $t$ value for triangle intersection
		Float invDet = 1 / det;
		Float b0 = e0 * invDet;
		Float b1 = e1 * invDet;
		Float b2 = e2 * invDet;
		Float t = tScaled * invDet;

		// Ensure that computed triangle $t$ is conservatively greater than zero

		// Compute $\delta_z$ term for triangle $t$ error bounds
		Float maxZt = maxComponent(abs(AVector3f(p0t.z, p1t.z, p2t.z)));
		Float deltaZ = gamma(3) * maxZt;

		// Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
		Float maxXt = maxComponent(abs(AVector3f(p0t.x, p1t.x, p2t.x)));
		Float maxYt = maxComponent(abs(AVector3f(p0t.y, p1t.y, p2t.y)));
		Float deltaX = gamma(5) * (maxXt + maxZt);
		Float deltaY = gamma(5) * (maxYt + maxZt);

		// Compute $\delta_e$ term for triangle $t$ error bounds
		Float deltaE = 2 * (gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);

		// Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
		Float maxE = maxComponent(abs(AVector3f(e0, e1, e2)));
		Float deltaT = 3 * (gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) * glm::abs(invDet);
		if (t <= deltaT)
			return false;

		return true;
	}

	bool ATriangleShape::hit(const ARay &ray, Float &tHit, ASurfaceInteraction &isect) const
	{
		// Get triangle vertices in _p0_, _p1_, and _p2_
		const auto &p0 = m_mesh->getPosition(m_indices[0]);
		const auto &p1 = m_mesh->getPosition(m_indices[1]);
		const auto &p2 = m_mesh->getPosition(m_indices[2]);

		// Perform ray--triangle intersection test

		// Transform triangle vertices to ray coordinate space

		// Translate vertices based on ray origin
		AVector3f p0t = p0 - AVector3f(ray.origin());
		AVector3f p1t = p1 - AVector3f(ray.origin());
		AVector3f p2t = p2 - AVector3f(ray.origin());

		// Permute components of triangle vertices and ray direction
		int kz = maxDimension(abs(ray.direction()));
		int kx = kz + 1;
		if (kx == 3) kx = 0;
		int ky = kx + 1;
		if (ky == 3) ky = 0;
		AVector3f d = permute(ray.direction(), kx, ky, kz);
		p0t = permute(p0t, kx, ky, kz);
		p1t = permute(p1t, kx, ky, kz);
		p2t = permute(p2t, kx, ky, kz);

		// Apply shear transformation to translated vertex positions
		Float Sx = -d.x / d.z;
		Float Sy = -d.y / d.z;
		Float Sz = 1.f / d.z;
		p0t.x += Sx * p0t.z;
		p0t.y += Sy * p0t.z;
		p1t.x += Sx * p1t.z;
		p1t.y += Sy * p1t.z;
		p2t.x += Sx * p2t.z;
		p2t.y += Sy * p2t.z;

		// Compute edge function coefficients _e0_, _e1_, and _e2_
		Float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
		Float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
		Float e2 = p0t.x * p1t.y - p0t.y * p1t.x;

		// Fall back to double precision test at triangle edges
		if (sizeof(Float) == sizeof(float) &&
			(e0 == 0.0f || e1 == 0.0f || e2 == 0.0f))
		{
			double p2txp1ty = (double)p2t.x * (double)p1t.y;
			double p2typ1tx = (double)p2t.y * (double)p1t.x;
			e0 = (float)(p2typ1tx - p2txp1ty);
			double p0txp2ty = (double)p0t.x * (double)p2t.y;
			double p0typ2tx = (double)p0t.y * (double)p2t.x;
			e1 = (float)(p0typ2tx - p0txp2ty);
			double p1txp0ty = (double)p1t.x * (double)p0t.y;
			double p1typ0tx = (double)p1t.y * (double)p0t.x;
			e2 = (float)(p1typ0tx - p1txp0ty);
		}

		// Perform triangle edge and determinant tests
		if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
			return false;
		Float det = e0 + e1 + e2;
		if (det == 0)
			return false;

		// Compute scaled hit distance to triangle and test against ray $t$ range
		p0t.z *= Sz;
		p1t.z *= Sz;
		p2t.z *= Sz;
		Float tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
		if (det < 0 && (tScaled >= 0 || tScaled < ray.m_tMax * det))
			return false;
		else if (det > 0 && (tScaled <= 0 || tScaled > ray.m_tMax * det))
			return false;

		// Compute barycentric coordinates and $t$ value for triangle intersection
		Float invDet = 1 / det;
		Float b0 = e0 * invDet;
		Float b1 = e1 * invDet;
		Float b2 = e2 * invDet;
		Float t = tScaled * invDet;

		// Ensure that computed triangle $t$ is conservatively greater than zero

		// Compute $\delta_z$ term for triangle $t$ error bounds
		Float maxZt = maxComponent(abs(AVector3f(p0t.z, p1t.z, p2t.z)));
		Float deltaZ = gamma(3) * maxZt;

		// Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
		Float maxXt = maxComponent(abs(AVector3f(p0t.x, p1t.x, p2t.x)));
		Float maxYt = maxComponent(abs(AVector3f(p0t.y, p1t.y, p2t.y)));
		Float deltaX = gamma(5) * (maxXt + maxZt);
		Float deltaY = gamma(5) * (maxYt + maxZt);

		// Compute $\delta_e$ term for triangle $t$ error bounds
		Float deltaE = 2 * (gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);

		// Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
		Float maxE = maxComponent(abs(AVector3f(e0, e1, e2)));
		Float deltaT = 3 * (gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) * glm::abs(invDet);
		if (t <= deltaT)
			return false;

		// Compute triangle partial derivatives
		AVector3f dpdu, dpdv;
		AVector2f uv[3];
		if (m_mesh->hasUV())
		{
			uv[0] = m_mesh->getUV(m_indices[0]);
			uv[1] = m_mesh->getUV(m_indices[1]);
			uv[2] = m_mesh->getUV(m_indices[2]);
		}
		else
		{
			uv[0] = AVector2f(0, 0);
			uv[1] = AVector2f(1, 0);
			uv[2] = AVector2f(1, 1);
		}

		// Compute deltas for triangle partial derivatives
		AVector2f duv02 = uv[0] - uv[2], duv12 = uv[1] - uv[2];
		AVector3f dp02 = p0 - p2, dp12 = p1 - p2;
		Float determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
		bool degenerateUV = glm::abs(determinant) < 1e-8;
		if (!degenerateUV)
		{
			Float invdet = 1 / determinant;
			dpdu = (duv12[1] * dp02 - duv02[1] * dp12) * invdet;
			dpdv = (-duv12[0] * dp02 + duv02[0] * dp12) * invdet;
		}
		if (degenerateUV || lengthSquared(cross(dpdu, dpdv)) == 0)
		{
			// Handle zero determinant for triangle partial derivative matrix
			AVector3f ng = cross(p2 - p0, p1 - p0);
			// The triangle is actually degenerate; the intersection is bogus.
			if (lengthSquared(ng) == 0)
				return false;

			coordinateSystem(normalize(ng), dpdu, dpdv);
		}

		// Interpolate $(u,v)$ parametric coordinates and hit point
		AVector3f pHit = b0 * p0 + b1 * p1 + b2 * p2;
		AVector2f uvHit = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];

		// Fill in _SurfaceInteraction_ from triangle hit
		isect = ASurfaceInteraction(pHit, uvHit, -ray.direction(), dpdu, dpdv, this);

		// Override surface normal in _isect_ for triangle
		isect.n = AVector3f(normalize(cross(dp02, dp12)));
		tHit = t;

		if (m_mesh->hasNormal())
		{
			AVector3f ns;
			ns = b0 * m_mesh->getNormal(m_indices[0]) + b1 * m_mesh->getNormal(m_indices[1])
				+ b2 * m_mesh->getNormal(m_indices[2]);
			if (lengthSquared(ns) > 0)
			{
				ns = normalize(ns);
			}
			else
			{
				ns = isect.n;
			}
			isect.n = ns;
		}

		return true;
	}

	Float ATriangleShape::solidAngle(const AVector3f &p, int nSamples) const
	{
		// Project the vertices into the unit sphere around p.
		const auto &p0 = m_mesh->getPosition(m_indices[0]);
		const auto &p1 = m_mesh->getPosition(m_indices[1]);
		const auto &p2 = m_mesh->getPosition(m_indices[2]);
		std::array<AVector3f, 3> pSphere = { normalize(p0 - p), normalize(p1 - p), normalize(p2 - p) };

		// http://math.stackexchange.com/questions/9819/area-of-a-spherical-triangle
		// Girard's theorem: surface area of a spherical triangle on a unit
		// sphere is the 'excess angle' alpha+beta+gamma-pi, where
		// alpha/beta/gamma are the interior angles at the vertices.
		//
		// Given three vertices on the sphere, a, b, c, then we can compute,
		// for example, the angle c->a->b by
		//
		// cos theta =  Dot(Cross(c, a), Cross(b, a)) /
		//              (Length(Cross(c, a)) * Length(Cross(b, a))).
		//
		AVector3f cross01 = (cross(pSphere[0], pSphere[1]));
		AVector3f cross12 = (cross(pSphere[1], pSphere[2]));
		AVector3f cross20 = (cross(pSphere[2], pSphere[0]));

		// Some of these vectors may be degenerate. In this case, we don't want
		// to normalize them so that we don't hit an assert. This is fine,
		// since the corresponding dot products below will be zero.
		if (lengthSquared(cross01) > 0)
			cross01 = normalize(cross01);
		if (lengthSquared(cross12) > 0)
			cross12 = normalize(cross12);
		if (lengthSquared(cross20) > 0)
			cross20 = normalize(cross20);

		// We only need to do three cross products to evaluate the angles at
		// all three vertices, though, since we can take advantage of the fact
		// that Cross(a, b) = -Cross(b, a).
		return glm::abs(
			glm::acos(clamp(dot(cross01, -cross12), -1, 1)) +
			glm::acos(clamp(dot(cross12, -cross20), -1, 1)) +
			glm::acos(clamp(dot(cross20, -cross01), -1, 1)) - aPi);
	}
}