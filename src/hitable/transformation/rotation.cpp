#include "rotation.h"

rt::Rotation::Rotation(std::shared_ptr<IHitable> hitable, const vec3& axis, float angle) 
	: m_hitable(hitable), m_axis(axis), m_theta(angle * DEG_TO_RAD) {
	aabb m_bounds, newbounds;
	m_hasbounds = m_hitable->boundingbox(m_bounds);

	// get all 8 points of the bounding box and rotate them
	// use the rotated points to create a new bounding box
	for (size_t z = 0; z < 2; ++z) {
		for (size_t y = 0; y < 2; ++y) {
			for (size_t x = 0; x < 2; ++x) {
				vec3 alpha(x, y, z);
				vec3 p = alpha * m_bounds.max() + (vec3(1) - alpha) * m_bounds.min();

				// rotate the point
				vec3 np = rotate(p, m_theta);

				// extend the new bounding box by the rotated point
				newbounds.extend(np);
			}
		}
	}

	// set the rotated bounds as bounds of this hitable
	m_bounds = newbounds;
}

bool rt::Rotation::hit(const ray& r, double tmin, double tmax, HitRecord& record) const {
	// rotate ray in the opposite direction
	vec3 o = rotate(r.o, -m_theta);
	vec3 dir = rotate(r.dir, -m_theta);
	ray rotatedray(o, dir);

	// hit hitable with rotated ray
	if (m_hitable->hit(rotatedray, tmin, tmax, record)) {
		// rotate intersection and normal back
		record.p = rotate(record.p, m_theta);
		record.normal = rotate(record.normal, m_theta);
		record.lp + rotate(record.lp, m_theta);

		return true;
	}

	return false;
}
bool rt::Rotation::boundingbox(aabb& box) const {
	if (m_hasbounds) {
		box = m_bounds;
		return true;
	}

	return false;
}

rt::vec3 rt::Rotation::rotate(const vec3& v, float theta) const {
	return std::cos(theta)* v + std::sin(theta) * cross(m_axis, v) + (1.0 - std::cos(theta)) * dot(m_axis, v) * m_axis;
}