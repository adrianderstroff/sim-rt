#ifndef HITABLE_H
#define HITABLE_H

#include "scene/ray.h"
#include "spatial/aabb.h"

namespace rt {
class IMaterial;

struct HitRecord {
    double t;
    vec3 p;
    vec3 normal;
	float u, v;
	std::shared_ptr<IMaterial> material;
};

class IHitable {
public:
	/**
	 * checks for an intersection between the ray and a hitable in the interval [tmin, tmax]
	 * @param r - ray to test the intersection for
	 * @param tmin - minimal allowed parameter t
	 * @param tmax - maximal allowed parameter t
	 * @param rec - intersection information of the hitable
	 * @return true if the ray intersects the hitable in the given interval, false otherwise
	 */
    virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
	/**
	 * retrieves the axis aligned bounding box of a hitable
	 * @param box - the retrieved bounding box
	 * @return true if the hitable has a bounding box, false otherwise
	 */
	virtual bool boundingbox(aabb& box) const = 0;
};
}

#endif//HITABLE_H