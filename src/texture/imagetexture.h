#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include "itexture.h"
#include "io/image.h"

namespace rt {
	/**
	 * an image texture takes an image and samples from this image
	 * the way the texture samples from the image depends on the 
	 * specified interpolation method. by default the image texture
	 * uses bilinear interpolation to create a smooth image
	 */
	class ImageTexture : public ITexture {
	public:
		/**
		 * interpolation type determines how to retrieve a
		 * value of a continues texture position from a
		 * discrete image
		 */
		enum Interpolation {
			NEAREST_NEIGHBOR,
			BILINEAR
		};
		/**
		 * wrap type determines how to handle values outside
		 * of the image bounds
		 */
		enum Wrap {
			REPEAT,
			CLAMP
		};

		ImageTexture(const Image& image) 
			: m_image(image), m_interpolationmethod(BILINEAR), m_wrapx(CLAMP), m_wrapy(CLAMP) {}

		/**
		 * setter for the interpolation method
		 * @param interpolation - interpolation method
		 */
		void set_interpolation_method(const Interpolation& interpolation) {
			m_interpolationmethod = interpolation;
		}
		/**
		 * setter for the wrap method
		 * @param wrapx - wrap method in x direction
		 * @param wrapy - wrap method in y direction
		 */
		void set_wrap_method(const Wrap& wrapx, const Wrap& wrapy) {
			m_wrapx = wrapx;
			m_wrapy = wrapy;
		}

		/**
		 * calculates the color for the continuous position (u,v)
		 * and local object position p
		 * @param u - horizontal coordinate in the range [0,1]
		 * @param v - vertical coordinate in the range [0,1]
		 * @param p - local position of the intersection point
		 * @return color value for the position (u,v)
		 */
		vec3 value(float u, float v, const vec3& p) const override {
			vec3 color;
			
			switch (m_interpolationmethod) {
			case BILINEAR:
				color = bilinear(u, v);
				break;
			case NEAREST_NEIGHBOR:
				color = nearest_neighbor(u, v);
				break;
			}

			return color;
		}

	private:
		Image m_image;
		Interpolation m_interpolationmethod;
		Wrap m_wrapx, m_wrapy;

		/**
		 * gets the color value of the discrete pixel
		 * position closest to (u,v)
		 */
		vec3 nearest_neighbor(float u, float v) const {
			int x = u * (m_image.width() - 1);
			int y = v * (m_image.height() - 1);
			handle_border(x, y);
			return m_image.get(x, y);
		}

		/**
		 * takes the color values of the 4 closest color
		 * values and interpolates between them depending
		 * on their distance to the position (u,v)
		 */
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

			// handle border cases
			handle_border(x0, y0);
			handle_border(x1, y1);

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

		/**
		 * handles values outside of the image dimensions
		 * and updates x and y accordingly to the Wrap method
		 */
		void handle_border(int& x, int& y) const {
			// handle x direction
			if (x < 0) {
				if (m_wrapx == Wrap::CLAMP) x = 0;
				if (m_wrapx == Wrap::REPEAT) while (x < 0) { x = m_image.width() + x; }
			}
			else if (x >= m_image.width()) {
				if (m_wrapx == Wrap::CLAMP) x = m_image.width() - 1;
				if (m_wrapx == Wrap::REPEAT) while (x >= m_image.width()) { x = x - m_image.width(); }
			}

			// handle y direction
			if (y < 0) {
				if (m_wrapy == Wrap::CLAMP) y = 0;
				if (m_wrapy == Wrap::REPEAT) while (y < 0) { y = m_image.height() + y; }
			}
			else if (y >= m_image.height()) {
				if (m_wrapy == Wrap::CLAMP) y = m_image.height() - 1;
				if (m_wrapy == Wrap::REPEAT) while (y >= m_image.height()) { y = y - m_image.height(); }
			}
		}
	};
}

#endif//IMAGE_TEXTURE_H