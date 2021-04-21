#ifndef MODELHITABLE_H
#define MODELHITABLE_H

#include "MeshHitable.h"

/**
 * @projectName   RayTracer
 * @brief         Model loader.
 * @author        YangWC
 * @date          2019-05-12
 */

namespace RayTracer
{

	class ModelHitable : public MeshHitable
	{
	private:
		Vector3D m_center;
		Vector3D m_scale;

	public:
		ModelHitable(const std::string &path, Vector3D pos, Vector3D len, unsigned int mat);
		virtual ~ModelHitable() = default;

		virtual float pdfValue(const Vector3D &o, const Vector3D &v) const override { return 0.0f; }
		virtual Vector3D random(const Vector3D &o) const override { return Vector3D(1.0f, 0.0f, 0.0f); }

	private:
		// Obj file loader.
		void loadObjFile(const std::string &path);
	};

}

#endif // MODELHITABLE_H
