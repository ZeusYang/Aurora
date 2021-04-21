#ifndef VECTOR3D_H
#define VECTOR3D_H

#include "MathUtils.h"
#include "Vector2D.h"

/**
 * @projectName   SoftRenderer
 * @brief         3D vector class.
 * @author        YangWC
 * @date          2019-04-28
 */
namespace RayTracer
{

	class Vector4D;
	class Vector3D
	{
	public:
		float x, y, z;

		// constructors
		Vector3D() :x(0.0f), y(0.0f), z(0.0f) {}
		Vector3D(float newX, float newY, float newZ) :x(newX), y(newY), z(newZ) {}
		Vector3D(const float * rhs) :x(*rhs), y(*(rhs + 1)), z(*(rhs + 2)) {}
		Vector3D(const Vector3D &rhs) :x(rhs.x), y(rhs.y), z(rhs.z) {}
		Vector3D(const Vector4D &rhs);
		~Vector3D() = default;

		// setter,getter
		void set(float newX, float newY, float newZ) { x = newX; y = newY; z = newZ; }
		void setX(float newX) { x = newX; }
		void setY(float newY) { y = newY; }
		void setZ(float newZ) { z = newZ; }
		float getX() const { return x; }
		float getY() const { return y; }
		float getZ() const { return z; }

		// normalization
		void normalize();
		Vector3D getNormalized() const;

		// length caculation
		float getLength() const { return static_cast<float>(sqrt(x*x + y * y + z * z)); }
		float getSquaredLength() const { return x * x + y * y + z * z; }

		// product
		float dotProduct(const Vector3D &rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
		Vector3D crossProduct(const Vector3D &rhs) const
		{
			return Vector3D(y*rhs.z - z * rhs.y, z*rhs.x - x * rhs.z, x*rhs.y - y * rhs.x);
		}

		// linear interpolation
		Vector3D lerp(const Vector3D &v2, float factor) const { return (*this)*(1.0f - factor) + v2 * factor; }
		Vector3D QuadraticInterpolate(const Vector3D &v2, const Vector3D &v3, float factor) const
		{
			return (*this)*(1.0f - factor)*(1.0f - factor) + v2 * 2.0f*factor*(1.0f - factor) + v3 * factor*factor;
		}

		// overloaded operators
		Vector3D operator+(const Vector3D &rhs) const { return Vector3D(x + rhs.x, y + rhs.y, z + rhs.z); }
		Vector3D operator-(const Vector3D &rhs) const { return Vector3D(x - rhs.x, y - rhs.y, z - rhs.z); }
		Vector3D operator*(const float rhs) const { return Vector3D(x*rhs, y*rhs, z*rhs); }
		Vector3D operator*(const Vector3D &rhs)const { return Vector3D(x*rhs.x, y*rhs.y, z*rhs.z); }
		Vector3D operator/(const float rhs) const { return (equal(rhs, 0.0f)) ? Vector3D(0.0f, 0.0f, 0.0f) : Vector3D(x / rhs, y / rhs, z / rhs); }
		Vector3D operator/(const Vector3D &rhs)const { return Vector3D(x / rhs.x, y / rhs.y, z / rhs.z); }

		bool operator==(const Vector3D &rhs) const { return (equal(x, rhs.x) && equal(y, rhs.y) && equal(z, rhs.z)); }
		bool operator!=(const Vector3D &rhs) const { return !((*this) == rhs); }

		void operator+=(const Vector3D &rhs) { x += rhs.x; y += rhs.y; z += rhs.z; }
		void operator-=(const Vector3D &rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; }
		void operator*=(const float rhs) { x *= rhs; y *= rhs; z *= rhs; }
		void operator*=(const Vector3D &rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; }
		void operator/=(const float rhs) { if (!equal(rhs, 0.0f)) { x /= rhs; y /= rhs; z /= rhs; } }
		void operator/=(const Vector3D &rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; }

		Vector3D operator-() const { return Vector3D(-x, -y, -z); }
		Vector3D operator+() const { return *this; }

		static Vector3D randomInUnitSphere()
		{
			Vector3D pos;
			do
			{
				pos = Vector3D(drand48(), drand48(), drand48()) * 2.0f - Vector3D(1.0, 1.0, 1.0);
			} while (pos.getSquaredLength() >= 1.0);
			return pos;
		}

		static Vector3D randomToSphere(float radius, float distance_squared)
		{
			float r1 = drand48();
			float r2 = drand48();
			float z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);
			float phi = 2 * M_PI * r1;
			float x = cos(phi) * sqrt(1 - z * z);
			float y = sin(phi) * sqrt(1 - z * z);
			return Vector3D(x, y, z);
		}

		static Vector3D reflect(const Vector3D &ray, const Vector3D &normal)
		{
			return ray - normal * (ray.dotProduct(normal)) * 2.0f;
		}

		static bool refract(const Vector3D &ray, const Vector3D &normal,
			float niOvernt, Vector3D &refracted)
		{
			Vector3D uv = ray;
			uv.normalize();
			float dt = (uv).dotProduct(normal);
			float discriminant = 1.0f - niOvernt * niOvernt * (1.0f - dt * dt);
			if (discriminant > 0.0f)
			{
				refracted = (uv - normal * dt) * niOvernt - normal * sqrt(discriminant);
				return true;
			}
			else
				return false;
		}

		static void getSphereUV(const Vector3D &p, Vector2D &tex)
		{
			float phi = atan2(p.z, p.x);
			float theta = asin(p.y);
			tex.x = 1 - (phi + M_PI) / (2 * M_PI);
			tex.y = (theta + M_PI / 2) / M_PI;
		}

		static Vector3D randomCosineDir()
		{
			float r1 = drand48();
			float r2 = drand48();
			float z = sqrt(1 - r2);
			float phi = 2 * M_PI * r1;
			float x = cos(phi) * 2 * sqrt(r2);
			float y = sin(phi) * 2 * sqrt(r2);
			return Vector3D(x, y, z);
		}
	};

}

#endif // VECTOR3D_H
