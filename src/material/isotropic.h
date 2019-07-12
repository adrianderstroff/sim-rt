#ifndef ISOTROPIC_H
#define ISOTROPIC_H

#include "imaterial.h"
#include "texture/itexture.h"

namespace rt {
	class Isotropic : public IMaterial {
	public:
		Isotropic(std::shared_ptr<ITexture> albedo) : m_albedo(albedo) {}

		virtual bool scatter(const ray& rIn, const HitRecord& record, vec3& attenuation, ray& scattered) const {
			scattered = ray(record.p, randomDir());
			attenuation = m_albedo->value(record.u, record.v, record.lp);
			return true;
		}

	private:
		std::shared_ptr<ITexture> m_albedo;
	};
}

#endif//ISOTROPIC_H