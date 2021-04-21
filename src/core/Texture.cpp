#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

namespace Aurora
{

	ImageTexture::ImageTexture(const std::string &path)
	{
		m_data = stbi_load(path.c_str(), &m_width, &m_height, &m_channel, 0);
		if (m_data == nullptr)
			std::cout << "Failed to load the image->" << path.c_str() << std::endl;
	}

	ImageTexture::~ImageTexture()
	{
		stbi_image_free(m_data);
	}

	AVector3f ImageTexture::sample(const Float &u, const Float &v, const AVector3f &p) const
	{
		int i = static_cast<int>(u * m_width);
		int j = static_cast<int>((1.0f - v)*m_height) - 0.001;
		if (i < 0) i = 0;
		if (j < 0) j = 0;
		if (i > m_width - 1) i = m_width - 1;
		if (j > m_height - 1) j = m_height - 1;
		int index = (j * m_width + i) * m_channel;
		Float r = static_cast<int>(m_data[index + 0]) / 255.0f;
		Float g = static_cast<int>(m_data[index + 1]) / 255.0f;
		Float b = static_cast<int>(m_data[index + 2]) / 255.0f;
		return AVector3f(r, g, b);
	}

}
