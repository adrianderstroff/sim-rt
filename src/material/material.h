#ifndef MATERIAL_H
#define MATERIAL_H

#include "hitable/hitable.h"

namespace rt {
class IMaterial {
public:
	virtual bool scatter(const ray& rIn, const HitRecord& rec, vec3& attenuation, ray& scattered) const = 0;
};
}

#endif//MATERIAL_H
