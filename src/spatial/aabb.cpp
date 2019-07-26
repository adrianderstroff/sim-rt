#include "aabb.h"

void rt::aabb::extend(const vec3& v) {
	m_min = rt::min(m_min, v);
	m_max = rt::max(m_max, v);
}

bool rt::aabb::hit(const ray& r, double tmin, double tmax) const {
	for (size_t i = 0; i < 3; ++i) {
		double invdir = 1.f / r.dir[i];
		double t0 = (m_min[i] - r.o[i]) * invdir;
		double t1 = (m_max[i] - r.o[i]) * invdir;
		if (invdir < 0.f) std::swap(t0, t1);
		
		tmin = std::max(tmin, t0);
		tmax = std::min(tmax, t1);
		if (tmax < tmin) return false;
	}
	return true;
}

void rt::aabb::surround(const aabb& box) {
	m_min = rt::min(m_min, box.min());
	m_max = rt::max(m_max, box.max());
}

rt::aabb rt::surrounding_box(const aabb& box1, const aabb& box2) {
	vec3 newmin = rt::min(box1.min(), box2.min());
	vec3 newmax = rt::max(box1.max(), box2.max());
	return aabb(newmin, newmax);
}