#ifndef SIMPLEMESHHITABLE_H
#define SIMPLEMESHHITABLE_H

#include "MeshHitable.h"

/**
 * @projectName   RayTracer
 * @brief         Some simple meshes.
 * @author        YangWC
 * @date          2019-05-12
 */

namespace RayTracer
{

	class Triangle : public MeshHitable
	{
	public:
		Triangle(Vector3D pos, Vector3D len, unsigned int mat);
		virtual ~Triangle() = default;
	};

	class Cube : public MeshHitable
	{
	public:
		Cube(Vector3D pos, Vector3D len, unsigned int mat);
		virtual ~Cube() = default;
	};

	class Plane : public MeshHitable
	{
	private:
		std::string m_name;

	public:
		Plane(Vector3D pos, Vector3D len, unsigned int mat);
		virtual ~Plane() = default;

		void setName(std::string target) { m_name = target; }

		virtual float pdfValue(const Vector3D &o, const Vector3D &v) const override;
		virtual Vector3D random(const Vector3D &o) const override;

		virtual std::string getName() const { return m_name; }

	};

}

#endif // SIMPLEMESHHITABLE_H
