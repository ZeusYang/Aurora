#include "ArFilm.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

namespace Aurora
{
	AURORA_REGISTER_CLASS(AFilm, "Film")

	AFilm::AFilm(const APropertyTreeNode &node)
	{
		const auto &props = node.getPropertyList();
		AVector2f _res = props.getVector2f("Resolution", AVector2f(800, 600));
		m_resolution = AVector2i(static_cast<int>(_res.x), static_cast<int>(_res.y));
		m_filename = props.getString("Filename", "rendered.png");

		AVector2f _cropMin = props.getVector2f("CropMin", AVector2f(0.0f));
		AVector2f _cropMax = props.getVector2f("CropMax", AVector2f(1.0f));
		//Compute film image bounds
		//Note: cropWindow range [0,1]x[0,1]
		m_croppedPixelBounds =
			ABounds2i(
				AVector2i(glm::ceil(m_resolution.x * _cropMin.x), glm::ceil(m_resolution.y * _cropMin.y)),
				AVector2i(glm::ceil(m_resolution.x * _cropMax.x), glm::ceil(m_resolution.y * _cropMax.y)));
		LOG(INFO) << "Created film with full resolution " << m_resolution <<
			". Crop window -> croppedPixelBounds " << m_croppedPixelBounds;

		m_diagonal = props.getFloat("Diagonal", 35.f);
		m_scale = props.getFloat("Scale", 1.0f);
		m_maxSampleLuminance = props.getFloat("MaxLum", aInfinity);

		//Filter
		{
			const auto &filterNode = node.getPropertyChild("Filter");
			m_filter = std::unique_ptr<AFilter>(static_cast<AFilter*>(AObjectFactory::createInstance(
				filterNode.getTypeName(), filterNode)));
		}

		initialize();
	}

	AFilm::AFilm(const AVector2i &resolution, const ABounds2f &cropWindow, std::unique_ptr<AFilter> filter,
		const std::string &filename, Float diagonal, Float scale, Float maxSampleLuminance)
		: m_resolution(resolution), m_filter(std::move(filter)), m_diagonal(diagonal),
		 m_filename(filename), m_scale(scale), m_maxSampleLuminance(maxSampleLuminance)
	{
		//Compute film image bounds
		//Note: cropWindow range [0,1]x[0,1]
		m_croppedPixelBounds =
			ABounds2i(
				AVector2i(glm::ceil(m_resolution.x * cropWindow.m_pMin.x),
					glm::ceil(m_resolution.y * cropWindow.m_pMin.y)),
				AVector2i(glm::ceil(m_resolution.x * cropWindow.m_pMax.x),
					glm::ceil(m_resolution.y * cropWindow.m_pMax.y)));
		LOG(INFO) << "Created film with full resolution " << resolution <<
			". Crop window of " << cropWindow << " -> croppedPixelBounds " << m_croppedPixelBounds;

		initialize();
	}

	void AFilm::initialize()
	{
		m_pixels = std::unique_ptr<APixel[]>(new APixel[m_croppedPixelBounds.area()]);

		//Precompute filter weight table
		//Note: we assume that filtering function f(x,y)=f(|x|,|y|)
		//      hence only store values for the positive quadrant of filter offsets.
		int offset = 0;
		for (int y = 0; y < filterTableWidth; ++y)
		{
			for (int x = 0; x < filterTableWidth; ++x, ++offset)
			{
				AVector2f p;
				p.x = (x + 0.5f) * m_filter->m_radius.x / filterTableWidth;
				p.y = (y + 0.5f) * m_filter->m_radius.y / filterTableWidth;
				m_filterTable[offset] = m_filter->evaluate(p);
			}
		}
	}

	ABounds2i AFilm::getSampleBounds() const
	{
		ABounds2f floatBounds(
			floor(AVector2f(m_croppedPixelBounds.m_pMin) + AVector2f(0.5f, 0.5f) - m_filter->m_radius),
			ceil(AVector2f(m_croppedPixelBounds.m_pMax) - AVector2f(0.5f, 0.5f) + m_filter->m_radius));
		return (ABounds2i)floatBounds;
	}

	std::unique_ptr<AFilmTile> AFilm::getFilmTile(const ABounds2i &sampleBounds)
	{
		// Bound image pixels that samples in _sampleBounds_ contribute to
		AVector2f halfPixel = AVector2f(0.5f, 0.5f);
		ABounds2f floatBounds = (ABounds2f)sampleBounds;
		AVector2i p0 = (AVector2i)ceil(floatBounds.m_pMin - halfPixel - m_filter->m_radius);
		AVector2i p1 = (AVector2i)floor(floatBounds.m_pMax - halfPixel + m_filter->m_radius) + AVector2i(1, 1);
		ABounds2i tilePixelBounds = intersect(ABounds2i(p0, p1), m_croppedPixelBounds);
		return std::unique_ptr<AFilmTile>(new AFilmTile(tilePixelBounds, m_filter->m_radius,
			m_filterTable, filterTableWidth, m_maxSampleLuminance));
	}

	void AFilm::mergeFilmTile(std::unique_ptr<AFilmTile> tile)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		for (AVector2i pixel : tile->getPixelBounds()) 
		{
			// Merge _pixel_ into _Film::pixels_
			const AFilmTilePixel &tilePixel = tile->getPixel(pixel);
			APixel &mergePixel = getPixel(pixel);
			Float xyz[3];
			tilePixel.m_contribSum.toXYZ(xyz);
			for (int i = 0; i < 3; ++i)
			{
				mergePixel.m_xyz[i] += xyz[i];
			}
			mergePixel.m_filterWeightSum += tilePixel.m_filterWeightSum;
		}
	}

	void AFilm::writeImageToFile(Float splatScale)
	{
		LOG(INFO) << "Converting image to RGB and computing final weighted pixel values";
		std::unique_ptr<Float[]> rgb(new Float[3 * m_croppedPixelBounds.area()]);
		std::unique_ptr<Byte[]>  dst(new Byte[3 * m_croppedPixelBounds.area()]);
		int offset = 0;
		for (AVector2i p : m_croppedPixelBounds) 
		{
			// Convert pixel XYZ color to RGB
			APixel &pixel = getPixel(p);
			XYZToRGB(pixel.m_xyz, &rgb[3 * offset]);

			// Normalize pixel with weight sum
			Float filterWeightSum = pixel.m_filterWeightSum;
			if (filterWeightSum != 0) 
			{
				Float invWt = (Float)1 / filterWeightSum;
				rgb[3 * offset + 0] = glm::max((Float)0, rgb[3 * offset + 0] * invWt);
				rgb[3 * offset + 1] = glm::max((Float)0, rgb[3 * offset + 1] * invWt);
				rgb[3 * offset + 2] = glm::max((Float)0, rgb[3 * offset + 2] * invWt);
			}

			// Add splat value at pixel
			Float splatRGB[3];
			Float splatXYZ[3] = { pixel.m_splatXYZ[0], pixel.m_splatXYZ[1],  pixel.m_splatXYZ[2] };
			XYZToRGB(splatXYZ, splatRGB);
			rgb[3 * offset + 0] += splatScale * splatRGB[0];
			rgb[3 * offset + 1] += splatScale * splatRGB[1];
			rgb[3 * offset + 2] += splatScale * splatRGB[2];

			// Scale pixel value by _scale_
			rgb[3 * offset + 0] *= m_scale;
			rgb[3 * offset + 1] *= m_scale;
			rgb[3 * offset + 2] *= m_scale;

#define TO_BYTE(v) (uint8_t) clamp(255.f * gammaCorrect(v) + 0.5f, 0.f, 255.f)
			dst[3 * offset + 0] = TO_BYTE(rgb[3 * offset + 0]);
			dst[3 * offset + 1] = TO_BYTE(rgb[3 * offset + 1]);
			dst[3 * offset + 2] = TO_BYTE(rgb[3 * offset + 2]);

			++offset;
		}

		LOG(INFO) << "Writing image " << m_filename << " with bounds " << m_croppedPixelBounds;
		auto extent = m_croppedPixelBounds.diagonal();
		stbi_write_png(m_filename.c_str(),
			extent.x,
			extent.y,
			3,
			static_cast<void*>(dst.get()),
			extent.x * 3);
	}

	void AFilm::setImage(const ASpectrum *img) const
	{
		int nPixels = m_croppedPixelBounds.area();
		for (int i = 0; i < nPixels; ++i) 
		{
			APixel &p = m_pixels[i];
			img[i].toXYZ(p.m_xyz);
			p.m_filterWeightSum = 1;
			p.m_splatXYZ[0] = p.m_splatXYZ[1] = p.m_splatXYZ[2] = 0;
		}
	}

	void AFilm::addSplat(const AVector2f &p, ASpectrum v)
	{
		//Note:Rather than computing the final pixel value as a weighted
		//     average of contributing splats, splats are simply summed.

		if (v.hasNaNs()) 
		{
			LOG(ERROR) << stringPrintf("Ignoring splatted spectrum with NaN values "
				"at (%f, %f)", p.x, p.y);
			return;
		}
		else if (v.y() < 0.) 
		{
			LOG(ERROR) << stringPrintf("Ignoring splatted spectrum with negative "
				"luminance %f at (%f, %f)", v.y(), p.x, p.y);
			return;
		}
		else if (glm::isinf(v.y())) 
		{
			LOG(ERROR) << stringPrintf("Ignoring splatted spectrum with infinite "
				"luminance at (%f, %f)", p.x, p.y);
			return;
		}

		AVector2i pi = AVector2i(floor(p));
		if (!insideExclusive(pi, m_croppedPixelBounds)) 
			return;

		if (v.y() > m_maxSampleLuminance)
		{
			v *= m_maxSampleLuminance / v.y();
		}

		Float xyz[3];
		v.toXYZ(xyz);

		APixel &pixel = getPixel(pi);
		for (int i = 0; i < 3; ++i)
		{
			pixel.m_splatXYZ[i].add(xyz[i]);
		}
	}

	void AFilm::clear()
	{
		for (AVector2i p : m_croppedPixelBounds) 
		{
			APixel &pixel = getPixel(p);
			for (int c = 0; c < 3; ++c)
			{
				pixel.m_splatXYZ[c] = pixel.m_xyz[c] = 0;
			}
			pixel.m_filterWeightSum = 0;
		}
	}
}