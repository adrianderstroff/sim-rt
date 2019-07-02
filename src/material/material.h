#ifndef MATERIAL_H
#define MATERIAL_H

#include "hitable/hitable.h"

namespace rt {
	/**
	 * interface for all materials. a material specifies which
	 * wavelengths get absorbed, how light gets reflected
	 * off the surface of an object and whether or not the
	 * material emits light
	 */
	class IMaterial {
	public:
		/**
		 * determines the scattered ray and the attenuation 
		 * of light after hitting the surface of the object
		 */
		virtual bool scatter(const ray& rIn, const HitRecord& rec, vec3& attenuation, ray& scattered) const = 0;
		/**
		 * returns the emitted light of this material
		 * @param u - the horizontal coordinate
		 * @param v - the vertical coordinate
		 * @return color for the position (u,v)
		 */
		virtual vec3 emitted(float u, float v) const { return vec3(0); }
	};
}

#endif//MATERIAL_H
