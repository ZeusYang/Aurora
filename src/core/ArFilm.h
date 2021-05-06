#ifndef ARFILM_H
#define ARFILM_H

#include "ArAurora.h"
#include "ArMathUtils.h"
#include "ArSpectrum.h"
#include "ArFilter.h"
#include "ArParallel.h"

#include <memory>
#include <vector>

namespace Aurora
{

	class AFilm final : public AObject
	{
	public:
		typedef std::shared_ptr<AFilm> ptr;

		AFilm(const APropertyTreeNode &node);
		AFilm(const AVector2i &resolution, const ABounds2f &cropWindow,
			std::unique_ptr<AFilter> filter, const std::string &filename, Float diagonal = 35.f,
			Float scale = 1.f, Float maxSampleLuminance = aInfinity);

		ABounds2i getSampleBounds() const;
		const AVector2i getResolution() const { return m_resolution; }

		std::unique_ptr<AFilmTile> getFilmTile(const ABounds2i &sampleBounds);
		void mergeFilmTile(std::unique_ptr<AFilmTile> tile);

		void writeImageToFile(Float splatScale = 1);

		void setImage(const ASpectrum *img) const;
		void addSplat(const AVector2f &p, ASpectrum v);

		void clear();

		virtual void activate() override 
		{
			m_filter->activate();
			initialize(); 
		}

		virtual AClassType getClassType() const override { return AClassType::AEFilm; }
		virtual std::string toString() const override { return "Film[]"; }

	private:
		void initialize();

	private:

		//Note: XYZ is a display independent representation of color,
		//      and this is why we choose to use XYZ color herein.
		struct APixel 
		{
			APixel() 
			{ 
				m_xyz[0] = m_xyz[1] = m_xyz[2] = m_filterWeightSum = 0; 
			}

			Float m_xyz[3];				//xyz color of the pixel
			Float m_filterWeightSum;	//the sum of filter weight values
			AAtomicFloat m_splatXYZ[3]; //unweighted sum of samples splats
			Float m_pad;				//unused, ensure sizeof(APixel) -> 32 bytes
		};

		AVector2i m_resolution; //(width, height)
		std::string m_filename;
		std::unique_ptr<APixel[]> m_pixels;

		Float m_diagonal;
		ABounds2i m_croppedPixelBounds;	//actual rendering window

		std::unique_ptr<AFilter> m_filter;
		std::mutex m_mutex;

		//Note: precomputed filter weights table
		static constexpr int filterTableWidth = 16;
		Float m_filterTable[filterTableWidth * filterTableWidth];

		Float m_scale;
		Float m_maxSampleLuminance;

		APixel &getPixel(const AVector2i &p)
		{
			CHECK(insideExclusive(p, m_croppedPixelBounds));
			int width = m_croppedPixelBounds.m_pMax.x - m_croppedPixelBounds.m_pMin.x;
			int index = (p.x - m_croppedPixelBounds.m_pMin.x) + (p.y - m_croppedPixelBounds.m_pMin.y) * width;
			return m_pixels[index];
		}

	};

	struct AFilmTilePixel
	{
		ASpectrum m_contribSum = 0.f;		//sum of the weighted spectrum contributions
		Float m_filterWeightSum = 0.f;		//sum of the filter weights
	};

	class AFilmTile final
	{
	public:
		// FilmTile Public Methods
		AFilmTile(const ABounds2i &pixelBounds, const AVector2f &filterRadius, const Float *filterTable,
			int filterTableSize, Float maxSampleLuminance)
			: m_pixelBounds(pixelBounds), m_filterRadius(filterRadius),
			m_invFilterRadius(1 / filterRadius.x, 1 / filterRadius.y),
			m_filterTable(filterTable), m_filterTableSize(filterTableSize),
			m_maxSampleLuminance(maxSampleLuminance) 
		{
			m_pixels = std::vector<AFilmTilePixel>(glm::max(0, pixelBounds.area()));
		}

		void addSample(const AVector2f &pFilm, ASpectrum L, Float sampleWeight = 1.f) 
		{
			if (L.y() > m_maxSampleLuminance)
				L *= m_maxSampleLuminance / L.y();

			// Compute sample's raster bounds
			AVector2f pFilmDiscrete = pFilm - AVector2f(0.5f, 0.5f);
			AVector2i p0 = (AVector2i)ceil(pFilmDiscrete - m_filterRadius);
			AVector2i p1 = (AVector2i)floor(pFilmDiscrete + m_filterRadius) + AVector2i(1, 1);
			p0 = max(p0, m_pixelBounds.m_pMin);
			p1 = min(p1, m_pixelBounds.m_pMax);

			// Loop over filter support and add sample to pixel arrays

			// Precompute $x$ and $y$ filter table offsets
			int *ifx = ALLOCA(int, p1.x - p0.x);
			for (int x = p0.x; x < p1.x; ++x) 
			{
				Float fx = glm::abs((x - pFilmDiscrete.x) * m_invFilterRadius.x * m_filterTableSize);
				ifx[x - p0.x] = glm::min((int)glm::floor(fx), m_filterTableSize - 1);
			}

			int *ify = ALLOCA(int, p1.y - p0.y);
			for (int y = p0.y; y < p1.y; ++y) 
			{
				Float fy = std::abs((y - pFilmDiscrete.y) * m_invFilterRadius.y * m_filterTableSize);
				ify[y - p0.y] = glm::min((int)std::floor(fy), m_filterTableSize - 1);
			}

			for (int y = p0.y; y < p1.y; ++y) 
			{
				for (int x = p0.x; x < p1.x; ++x) 
				{
					// Evaluate filter value at $(x,y)$ pixel
					int offset = ify[y - p0.y] * m_filterTableSize + ifx[x - p0.x];
					Float filterWeight = m_filterTable[offset];

					// Update pixel values with filtered sample contribution
					AFilmTilePixel &pixel = getPixel(AVector2i(x, y));
					pixel.m_contribSum += L * sampleWeight * filterWeight;
					pixel.m_filterWeightSum += filterWeight;
				}
			}
		}

		AFilmTilePixel &getPixel(const AVector2i &p) 
		{
			CHECK(insideExclusive(p, m_pixelBounds));
			int width = m_pixelBounds.m_pMax.x - m_pixelBounds.m_pMin.x;
			int index = (p.x - m_pixelBounds.m_pMin.x) + (p.y - m_pixelBounds.m_pMin.y) * width;
			return m_pixels[index];
		}

		const AFilmTilePixel &getPixel(const AVector2i &p) const 
		{
			CHECK(insideExclusive(p, m_pixelBounds));
			int width =m_pixelBounds.m_pMax.x - m_pixelBounds.m_pMin.x;
			int index = (p.x - m_pixelBounds.m_pMin.x) + (p.y - m_pixelBounds.m_pMin.y) * width;
			return m_pixels[index];
		}

		ABounds2i getPixelBounds() const { return m_pixelBounds; }

	private:
		const ABounds2i m_pixelBounds;
		const AVector2f m_filterRadius, m_invFilterRadius;
		const Float *m_filterTable;
		const int m_filterTableSize;
		std::vector<AFilmTilePixel> m_pixels;
		const Float m_maxSampleLuminance;
		
		friend class Film;
	};

}

#endif