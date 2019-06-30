#include "rectangle.h"

bool rt::Rectangle::hit(const ray& r, double tMin, double tMax, HitRecord& rec) const {
	HitRecord rec1, rec2;
	bool hit1 = m_t1.hit(r, tMin, tMax, rec1);
	bool hit2 = m_t2.hit(r, tMin, tMax, rec2);

	if (hit1 && hit2) rec = (rec1.t < rec2.t) ? rec1 : rec2;
	else if (hit1) rec = rec1;
	else if (hit2) rec = rec2;

	return hit1 || hit2;
}
bool rt::Rectangle::boundingbox(aabb& box) const {
	aabb box1, box2;
	m_t1.boundingbox(box1);
	m_t2.boundingbox(box2);
	box = surrounding_box(box1, box2);

	return true;
}