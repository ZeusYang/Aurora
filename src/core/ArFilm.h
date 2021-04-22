#ifndef ARFILM_H
#define ARFILM_H

#include "ArAurora.h"
#include "ArMathUtils.h"
#include "ArSpectrum.h"

#include <memory>

namespace Aurora
{
	class AFilm final
	{
	public:
		typedef std::shared_ptr<AFilm> ptr;

		AFilm(const AVector2i &resolution, const std::string &filename);

		void writeImageToFile();

		void clear();

		void setSpectrum(const AVector2i &pFilm, const ASpectrum &Li);

		const AVector2i getResolution() const { return m_resolution; }

	private:
		const AVector2i m_resolution; //(width, height)
		const std::string m_filename;
		std::unique_ptr<ARGBSpectrum[]> m_pixels;

		ARGBSpectrum &getPixel(const AVector2i &p)
		{
			int index = p.y * m_resolution.x + p.x;
			return m_pixels[index];
		}

	};

}

#endif