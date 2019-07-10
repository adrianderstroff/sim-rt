#ifndef I_TEXTURE_H
#define I_TEXTURE_H

#include "math/vec3.h"

namespace rt {
	class ITexture {
	public:
		/**
		 * calculates the color for the continuous position (u,v)
		 * and local object position p
		 * @param u - horizontal coordinate in the range [0,1]
		 * @param v - vertical coordinate in the range [0,1]
		 * @param p - local position of the intersection point
		 * @return color value for the position (u,v)
		 */
		virtual vec3 value(float u, float v, const vec3& p) const = 0;
	};
}

#endif//I_TEXTURE_H