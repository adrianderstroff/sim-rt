#ifndef TEXTURE_H
#define TEXTURE_H

#include "math/vec3.h"

namespace rt {
	class ITexture {
	public:
		virtual vec3 value(float u, float v) const = 0;
	};
}

#endif//TEXTURE_H