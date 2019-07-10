#ifndef DIFFUSE_LIGHT_H
#define DIFFUSE_LIGHT_H

#include "imaterial.h"
#include "texture/itexture.h"

namespace rt {
	/**
	 * a diffuse light emits light but doesn't scatter light
	 */
	class DiffuseLight : public IMaterial {
	public:
		DiffuseLight(std::shared_ptr<ITexture> texture) : m_emit(texture) {}

		/**
		 * determines the scattered ray and the attenuation
		 * of light after hitting the surface of the object
		 */
		virtual bool scatter(const ray& rIn, const HitRecord& rec, vec3& attenuation, ray& scattered) const override {
			return false;
		}

		/**
		 * returns the emitted light of this material
		 * @param u - the horizontal coordinate
		 * @param v - the vertical coordinate
		 * @return color for the position (u,v)
		 */
		virtual vec3 emitted(float u, float v, const vec3& lp) const override { 
			return m_emit->value(u, v, lp);
		}

	private:
		std::shared_ptr<ITexture> m_emit;
	};
}

#endif//DIFFUSE_LIGHT_H