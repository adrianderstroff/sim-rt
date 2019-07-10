#ifndef METAL_H
#define METAL_H

#include "imaterial.h"
#include "texture/itexture.h"

namespace rt {
class Metal : public IMaterial {
public:
	Metal(std::shared_ptr<ITexture> a) : m_albedo(a) {}
	virtual bool scatter(const ray& rIn, const HitRecord& rec, vec3& attenuation, ray& scattered) const {
		vec3 reflected = reflect(normalize(rIn.dir), rec.normal);
		scattered = ray(rec.p, reflected);
		attenuation = m_albedo->value(rec.u, rec.v, rec.lp);
		return (dot(scattered.dir, rec.normal) > 0);
	}

private:
	std::shared_ptr<ITexture> m_albedo;
};
}

#endif//METAL_H