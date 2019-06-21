#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "material.h"

class Lambertian : public Material {
public:
	Lambertian(const vec3& a) : m_albedo(a) {}
	virtual bool scatter(const ray& rIn, const HitRecord& rec, vec3& attenuation, ray& scattered) const {
		vec3 target = rec.p + rec.normal + randomDir();
		scattered = ray(rec.p, target - rec.p);
		attenuation = m_albedo;
		return true;
	}

private:
	vec3 m_albedo;
};

#endif//LAMBERTIAN_H
