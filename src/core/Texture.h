#ifndef TEXTURE_H
#define TEXTURE_H

#include "ArMathUtils.h"
namespace Aurora
{

	class Texture
	{
	public:
		typedef std::shared_ptr<Texture> ptr;

		Texture() = default;
		virtual ~Texture() = default;
		virtual AVector3f sample(const Float &u, const Float &v, const AVector3f &p) const = 0;
	};

	class ConstantTexture : public Texture
	{
	private:
		AVector3f m_color;

	public:
		typedef std::shared_ptr<ConstantTexture> ptr;

		ConstantTexture() = default;
		ConstantTexture(AVector3f c) : m_color(c) {}
		virtual ~ConstantTexture() = default;

		virtual AVector3f sample(const Float &u, const Float &v, const AVector3f &p) const
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

		virtual AVector3f sample(const Float &u, const Float &v, const AVector3f &p) const
		{
			Float sines = sin(10 * p.x)*sin(10 * p.y)*sin(10 * p.z);
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

		virtual AVector3f sample(const Float &u, const Float &v, const AVector3f &p) const;

	};

}

#endif // TEXTURE_H
