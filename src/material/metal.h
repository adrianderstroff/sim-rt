#ifndef METAL_H
#define METAL_H

#include "material.h"

class Metal : public Material {
public:
	Metal(const vec3& a) : m_albedo(a) {}
	virtual bool scatter(const ray& rIn, const HitRecord& rec, vec3& attenuation, ray& scattered) const {
		vec3 reflected = reflect(normalize(rIn.dir), rec.normal);
		scattered = ray(rec.p, reflected);
		attenuation = m_albedo;
		return (dot(scattered.dir, rec.normal) > 0);
	}

private:
	vec3 m_albedo;
};

#endif//METAL_H