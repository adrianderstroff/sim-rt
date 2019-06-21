#ifndef HITABLE_H
#define HITABLE_H

#include "scene/ray.h"

class Material;

struct HitRecord {
    double t;
    vec3 p;
    vec3 normal;
	std::shared_ptr<Material> material;
};

class Hitable {
public:
    virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
};

#endif//HITABLE_H