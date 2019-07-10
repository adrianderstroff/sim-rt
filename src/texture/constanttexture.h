#ifndef CONSTANT_TEXTURE_H
#define CONSTANT_TEXTURE_H

#include "itexture.h"

namespace rt {
	class ConstantTexture : public ITexture {
	public:
		ConstantTexture(const vec3& color) : m_color(color) {}
		
		/**
		 * calculates the color for the continuous position (u,v)
		 * and local object position p
		 * @param u - horizontal coordinate in the range [0,1]
		 * @param v - vertical coordinate in the range [0,1]
		 * @param p - local position of the intersection point
		 * @return color value for the position (u,v)
		 */
		vec3 value(float u, float v, const vec3& p) const override {
			return m_color;
		}

	private:
		vec3 m_color;
	};

	std::shared_ptr<ConstantTexture> create_color(const vec3& color) {
		return std::make_shared<ConstantTexture>(color);
	}
}

#endif//CONSTANT_TEXTURE_H