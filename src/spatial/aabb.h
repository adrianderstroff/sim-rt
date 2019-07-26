#ifndef AABB_H
#define AABB_H

#include "scene/ray.h"
#include "math/vec3.h"

namespace rt {
class aabb {
public:
	aabb() : m_min(DBL_MAX), m_max(-DBL_MAX) {}
	aabb(const vec3& vmin, const vec3& vmax) : m_min(vmin), m_max(vmax) {}

	vec3 min() const { return m_min; };
	vec3 max() const { return m_max; };
	vec3 center() const { return (m_min + m_max) / 2.f; }

	void extend(const vec3& v);
	void surround(const aabb& box);

	bool hit(const ray& r, double tmin, double tmax) const;

private:
	vec3 m_min;
	vec3 m_max;
};

aabb surrounding_box(const aabb& box1, const aabb& box2);
}

#endif//AABB_H