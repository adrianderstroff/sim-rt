#include "translation.h"

rt::Translation::Translation(std::shared_ptr<IHitable> hitable, const vec3& offset) 
	: m_hitable(hitable), m_offset(offset) { }

bool rt::Translation::hit(const ray& r, double tmin, double tmax, HitRecord& record) const {
	ray translatedray(r.o - m_offset, r.dir);
	if (m_hitable->hit(translatedray, tmin, tmax, record)) {
		record.p += m_offset;
		return true;
	}

	return false;
}
bool rt::Translation::boundingbox(aabb& box) const {
	if (m_hitable->boundingbox(box)) {
		box = aabb(box.min() + m_offset, box.max() + m_offset);
		return true;
	}

	return false;
}