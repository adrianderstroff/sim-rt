#ifndef CUBE_MAP_TEXTURE_H
#define CUBE_MAP_TEXTURE_H

#include "itexture.h"
#include "imagetexture.h"

namespace rt {
	/**
	 * an image texture takes an image and samples from this image
	 * the way the texture samples from the image depends on the
	 * specified interpolation method. by default the image texture
	 * uses bilinear interpolation to create a smooth image
	 */
	class CubeMap : public ITexture {
	public:
		enum CubeTextureIndex {
			RIGHT  = 0,
			LEFT   = 1,
			TOP    = 2,
			BOTTOM = 3,
			BACK   = 4,
			FRONT  = 5
		};

		/**
		 * constructor having six images for each side of a cube
		 */
		CubeMap(const Image& right, const Image& left, const Image& top, const Image& bottom, const Image& back, const Image& front) {
			m_imagetextures.push_back(std::make_shared<ImageTexture>(right ));
			m_imagetextures.push_back(std::make_shared<ImageTexture>(left  ));
			m_imagetextures.push_back(std::make_shared<ImageTexture>(top   ));
			m_imagetextures.push_back(std::make_shared<ImageTexture>(bottom));
			m_imagetextures.push_back(std::make_shared<ImageTexture>(back  ));
			m_imagetextures.push_back(std::make_shared<ImageTexture>(front ));
		}

		/**
		 * setter for the interpolation method
		 * @param interpolation - interpolation method
		 */
		void set_interpolation_method(const ImageTexture::Interpolation& interpolation) {
			for (auto& tex : m_imagetextures) {
				tex->set_interpolation_method(interpolation);
			}
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
			// get biggest dimension
			vec3 dir = normalize(p);
			float x = std::abs(dir.x);
			float y = std::abs(dir.y);
			float z = std::abs(dir.z);

			// determine which side of the cube had been hit
			vec3 color;
			if (x >= y && x >= z) {
				color = (dir.x < 0)
					? m_imagetextures.at(CubeTextureIndex::LEFT )->value(u, v, p) 
					: m_imagetextures.at(CubeTextureIndex::RIGHT)->value(u, v, p);
			}
			else if (y >= x && y >= z) {
				color = (dir.y < 0)
					? m_imagetextures.at(CubeTextureIndex::BOTTOM)->value(u, v, p)
					: m_imagetextures.at(CubeTextureIndex::TOP   )->value(u, v, p);
			}
			else {
				color = (dir.z < 0)
					? m_imagetextures.at(CubeTextureIndex::BACK )->value(u, v, p)
					: m_imagetextures.at(CubeTextureIndex::FRONT)->value(u, v, p);
			}

			return color;
		}

	private:
		std::vector<std::shared_ptr<ImageTexture>> m_imagetextures;
	};
}

#endif//CUBE_MAP_TEXTURE_H