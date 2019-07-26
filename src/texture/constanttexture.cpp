#include "constanttexture.h"

rt::vec3 rt::ConstantTexture::value(float u, float v, const vec3& p) const {
	return m_color;
}

std::shared_ptr<rt::ConstantTexture> rt::new_color(const vec3& color) {
	return std::make_shared<ConstantTexture>(color);
}