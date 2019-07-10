#include "hitablelist.h"

bool rt::HitableList::hit(const ray& r, double tmin, double tmax, HitRecord& rec) const {
	HitRecord tempRec;

	bool hitAnything = false;
	double closestSoFar = tmax;
	for (size_t i = 0; i < m_list.size(); i++) {
		if (m_list.at(i)->hit(r, tmin, closestSoFar, tempRec)) {
			hitAnything = true;
			closestSoFar = tempRec.t;
			rec = tempRec;
		}
	}

	return hitAnything;
}

bool rt::HitableList::boundingbox(aabb& box) const {
	// hitable list has no children  and thus no bounding box
	if (m_list.size() < 1) return false;

	// if the first element has no bounding box we can't determine
	// a bounding box that covers all elements
	aabb tempbox;
	if (!m_list.at(0)->boundingbox(tempbox)) return false;
	else box = tempbox;

	// extend bounding box for all other hitables or stop
	// if any of the hitables has no bounding box
	for (size_t i = 1; i < m_list.size(); ++i) {
		if (!m_list.at(i)->boundingbox(tempbox)) return false;
		else box = surrounding_box(box, tempbox);
	}

	return true;
}