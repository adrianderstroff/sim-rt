#include "rectangle.h"

rt::Rectangle::Rectangle(vec3 p, float width, float height, std::shared_ptr<IMaterial> mat) : m_position(p) {
	vec3 halfwidth = vec3(width / 2, 0, 0);
	vec3 halfheight = vec3(0, height / 2, 0);

	// determine positions
	vec3 p1 = p - halfwidth + halfheight;
	vec3 p2 = p - halfwidth - halfheight;
	vec3 p3 = p + halfwidth + halfheight;
	vec3 p4 = p + halfwidth - halfheight;

	// determine normal
	vec3 n = rt::normal(p1, p2, p3);

	// determine texture coordinates
	vec3 t1(0, 1, 0);
	vec3 t2(0, 0, 0);
	vec3 t3(1, 1, 0);
	vec3 t4(1, 0, 0);

	m_t1 = Triangle(p1, p2, p3, n, n, n, t1, t2, t3, mat);
	m_t2 = Triangle(p3, p2, p4, n, n, n, t3, t2, t4, mat);
}

rt::Rectangle::Rectangle(vec3 p, vec3 right, vec3 up, std::shared_ptr<IMaterial> mat) : m_position(p) {
	// determine positions
	vec3 p1 = p - right + up;
	vec3 p2 = p - right - up;
	vec3 p3 = p + right + up;
	vec3 p4 = p + right - up;

	// determine normal
	vec3 n = rt::normal(p1, p2, p3);

	// determine texture coordinates
	vec3 t1(0, 0, 0);
	vec3 t2(0, 1, 0);
	vec3 t3(1, 0, 0);
	vec3 t4(1, 1, 0);

	m_t1 = Triangle(p1, p2, p3, n, n, n, t1, t2, t3, mat);
	m_t2 = Triangle(p3, p2, p4, n, n, n, t3, t2, t4, mat);
}

bool rt::Rectangle::hit(const ray& r, double tMin, double tMax, HitRecord& rec) const {
	HitRecord rec1, rec2;
	bool hit1 = m_t1.hit(r, tMin, tMax, rec1);
	bool hit2 = m_t2.hit(r, tMin, tMax, rec2);

	if (hit1 && hit2) rec = (rec1.t < rec2.t) ? rec1 : rec2;
	else if (hit1) rec = rec1;
	else if (hit2) rec = rec2;
	bool anyhit = hit1 || hit2;

	// update local position of the intersection
	if (anyhit) rec.lp = rec.p - m_position;

	return anyhit;
}
bool rt::Rectangle::boundingbox(aabb& box) const {
	aabb box1, box2;
	m_t1.boundingbox(box1);
	m_t2.boundingbox(box2);
	box = surrounding_box(box1, box2);

	return true;
}