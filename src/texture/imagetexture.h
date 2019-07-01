#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include "texture.h"
#include "io/image.h"

namespace rt {
	class ImageTexture : public ITexture {
	public:
		ImageTexture(const Image& image) : m_image(image) {}

		vec3 value(float u, float v) const override {
			return bilinear(u, v);
		}

	private:
		Image m_image;

		vec3 nearest_neighbor(float u, float v) const {
			int x = u * (m_image.width() - 1);
			int y = v * (m_image.height() - 1);
			return m_image.get(x, y);
		}

		vec3 bilinear(float u, float v) const {
			// relative position within image space
			float fx = u * (m_image.width() - 1);
			float fy = v * (m_image.height() - 1);
			
			// relative position within pixel
			float rx = fx - std::floor(fx);
			float ry = fy - std::floor(fy);

			// pixel positions of the closest pixels
			int x0 = std::floor(fx);
			int x1 = std::ceil(fx);
			int y0 = std::floor(fy);
			int y1 = std::ceil(fy);

			// retrieve color of the pixels
			vec3 c00 = m_image.get(x0, y0);
			vec3 c10 = m_image.get(x1, y0);
			vec3 c01 = m_image.get(x0, y1);
			vec3 c11 = m_image.get(x1, y1);

			// perform bilinear interpolation
			vec3 cy0 = lerp(c00, c10, rx);
			vec3 cy1 = lerp(c01, c11, rx);
			vec3 color = lerp(cy0, cy1, ry);

			return color;
		}
	};
}

#endif//IMAGE_TEXTURE_H