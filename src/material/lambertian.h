#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "material.h"
#include "texture/texture.h"

namespace rt {
	class Lambertian : public IMaterial {
	public:
		Lambertian(std::shared_ptr<ITexture> a) : m_albedo(a) {}

		virtual bool scatter(const ray& rIn, const HitRecord& rec, vec3& attenuation, ray& scattered) const {
			vec3 target = rec.p + rec.normal + randomDir();
			scattered = ray(rec.p, target - rec.p);
			attenuation = m_albedo->value(rec.u, rec.v);
			return true;
		}

	private:
		std::shared_ptr<ITexture> m_albedo;
	};
}

#endif//LAMBERTIAN_H
