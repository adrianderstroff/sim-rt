#ifndef NORMAL_MATERIAL_H
#define NORMAL_MATERIAL_H

#include "material.h"

namespace rt {
class NormalMaterial : public IMaterial {
public:
	NormalMaterial() {}
	virtual bool scatter(const ray& rIn, const HitRecord& rec, vec3& attenuation, ray& scattered) const {
		vec3 target = rec.p + rec.normal + randomDir();
		scattered = ray(rec.p, normalize(target - rec.p));
		attenuation = (rec.normal + vec3(1.f)) / 2.f;
		return true;
	}
};
}

#endif//NORMAL_MATERIAL_H