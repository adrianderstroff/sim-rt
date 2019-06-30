#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include "texture.h"
#include "io/image.h"

namespace rt {
	class ImageTexture : public ITexture {
	public:
		ImageTexture(const Image& image) : m_image(image) {}

		vec3 value(float u, float v) const override {
			int x = u * (m_image.width() - 1);
			int y = v * (m_image.height() - 1);
			return m_image.get(x, y);
		}

	private:
		Image m_image;
	};
}

#endif//IMAGE_TEXTURE_H