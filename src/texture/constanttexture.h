#ifndef CONSTANT_TEXTURE_H
#define CONSTANT_TEXTURE_H

#include "texture.h"

namespace rt {
	class ConstantTexture : public ITexture {
	public:
		ConstantTexture(const vec3& color) : m_color(color) {}
		
		vec3 value(float u, float v) const override {
			return m_color;
		}

	private:
		vec3 m_color;
	};
}

#endif//CONSTANT_TEXTURE_H