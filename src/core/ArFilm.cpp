#include "ArFilm.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace Aurora
{
	AFilm::AFilm(const AVector2i &resolution, const std::string &filename)
		: m_resolution(resolution), m_filename(filename)
	{
		m_pixels = std::unique_ptr<ARGBSpectrum[]>(new ARGBSpectrum[resolution.x * resolution.y]);
	}

	void AFilm::setSpectrum(const AVector2i &pFilm, const ASpectrum &Li)
	{
		int index = pFilm.y * m_resolution.x + pFilm.x;
		m_pixels[index] = Li;
	}

	void AFilm::writeImageToFile()
	{
		std::unique_ptr<Byte[]> rgbImg(new Byte[3 * m_resolution.x * m_resolution.y]);

		int offset = 0;
		for (int y = 0; y < m_resolution.y; ++y)
		{
			for (int x = 0; x < m_resolution.x; ++x)
			{
				auto &pixel = getPixel(AVector2i(x, y));
				//Note: at present, no hdr -> ldr.
				rgbImg[3 * offset + 0] = static_cast<Byte>(glm::min(pixel[0] * 255, (Float)255));
				rgbImg[3 * offset + 1] = static_cast<Byte>(glm::min(pixel[1] * 255, (Float)255));
				rgbImg[3 * offset + 2] = static_cast<Byte>(glm::min(pixel[2] * 255, (Float)255));
				++offset;
			}
		}

		stbi_flip_vertically_on_write(1);
		stbi_write_png(m_filename.c_str(),
			m_resolution.x,
			m_resolution.y,
			3,
			static_cast<void*>(rgbImg.get()),
			m_resolution.x * 3);
	}

	void AFilm::clear()
	{
		for (int y = 0; y < m_resolution.y; ++y)
		{
			for (int x = 0; x < m_resolution.x; ++x)
			{
				AVector2i p(x, y);
				auto &pixel = getPixel(p);
				pixel[0] = pixel[1] = pixel[2] = 0;
			}
		}
	}
}