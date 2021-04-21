#ifndef TEXTURE_H
#define TEXTURE_H

#include "Vector3D.h"

/**
 * @projectName   RayTracer
 * @brief         Texture abstract class.
 * @author        YangWC
 * @date          2019-05-11
 */

namespace RayTracer
{

	class Texture
	{
	public:
		typedef std::shared_ptr<Texture> ptr;

		Texture() = default;
		virtual ~Texture() = default;
		virtual Vector3D sample(const float &u, const float &v, const Vector3D &p) const = 0;
	};

	class ConstantTexture : public Texture
	{
	private:
		Vector3D m_color;

	public:
		typedef std::shared_ptr<ConstantTexture> ptr;

		ConstantTexture() = default;
		ConstantTexture(Vector3D c) : m_color(c) {}
		virtual ~ConstantTexture() = default;

		virtual Vector3D sample(const float &u, const float &v, const Vector3D &p) const
		{
			(void)u;
			(void)v;
			(void)p;
			return m_color;
		}
	};

	class CheckerTexture : public Texture
	{
	private:
		Texture *m_odd, *m_even;

	public:
		typedef std::shared_ptr<CheckerTexture> ptr;

		CheckerTexture() = default;
		CheckerTexture(Texture *t0, Texture *t1) : m_even(t0), m_odd(t1) { }
		virtual ~CheckerTexture()
		{
			if (m_odd) delete m_odd;
			if (m_even) delete m_even;
			m_odd = m_even = nullptr;
		}

		virtual Vector3D sample(const float &u, const float &v, const Vector3D &p) const
		{
			float sines = sin(10 * p.x)*sin(10 * p.y)*sin(10 * p.z);
			if (sines < 0.0f)
				return m_odd->sample(u, v, p);
			else
				return m_even->sample(u, v, p);
		}

	};

	class ImageTexture : public Texture
	{
	private:
		unsigned char *m_data;
		int m_width, m_height, m_channel;

	public:
		typedef std::shared_ptr<ImageTexture> ptr;

		ImageTexture() = default;
		ImageTexture(const std::string &path);
		virtual ~ImageTexture();

		virtual Vector3D sample(const float &u, const float &v, const Vector3D &p) const;

	};

}

#endif // TEXTURE_H
