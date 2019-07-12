#include "triangle.h"

bool rt::Triangle::hit(const ray& r, double tmin, double tmax, HitRecord& rec) const {
	// moeller-trumbore algorithm
	vec3 v1 = p2 - p1;
	vec3 v2 = p3 - p1;
	vec3 pvec = cross(r.dir, v2);
	float det = dot(v1, pvec);

	// ray and triangle are parallel
	if (fabs(det) < rt::EPS) return false;
	//if (det < rt::EPS) return false;

	float invdet = 1.0f / det;

	// calculate u-coordinate
	vec3 tvec = r.o - p1;
	float u = dot(tvec, pvec) * invdet;
	if (u < 0.f || u > 1.f) return false;

	// calculate v-coordinate
	vec3 qvec = cross(tvec, v1);
	float v = dot(r.dir, qvec) * invdet;
	if (v < 0.f || u + v > 1.f) return false;

	// calculate t-coordinate
	float t = dot(v2, qvec) * invdet;

	if (t < tmin || t > tmax) return false;

	// determine barycentric coordinates
	vec3 p = r.position(t);
	vec3 bcoords = barycentric(p, p1, p2, p3);
	vec3 n = normalize(from_barycentric(bcoords, n1, n2, n3));
	vec3 uvw = from_barycentric(bcoords, t1, t2, t3);
	vec3 lp = (p1 + p2 + p3) / 3.f;

	rec.t = t;
	rec.p = p;
	rec.normal = n;
	rec.u = uvw.x; 
	rec.v = uvw.y;
	rec.lp = lp;
	rec.material = m_material;

	return true;
}
bool rt::Triangle::boundingbox(aabb& box) const {
	box = aabb();
	box.extend(p1);
	box.extend(p2);
	box.extend(p3);

	return true;
}

rt::vec3 rt::Triangle::barycentric(const vec3& x, const vec3& p1, const vec3& p2, const vec3& p3) const {
	vec3 vn = cross(p2 - p1, p3 - p1);
	float A = length(vn);
	vec3 n = vn / A;

	float u = dot(cross(p3 - p2, x - p2), n) / A;
	float v = dot(cross(x - p3, p3 - p1), n) / A;
	float w = 1 - u - v;
	return vec3(u, v, w);
}

rt::vec3 rt::Triangle::from_barycentric(const vec3& b, const vec3& p1, const vec3& p2, const vec3& p3) const {
	return b.x*p1 + b.y*p2 + b.z*p3;
}