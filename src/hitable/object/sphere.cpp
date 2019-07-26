#include "sphere.h"

bool rt::Sphere::hit(const ray& r, double tmin, double tmax, HitRecord& rec) const {
	// solve squared term
	vec3 oc = r.o - center;
	double a = dot(r.dir, r.dir);
	double b = dot(oc, r.dir);
	double c = dot(oc, oc) - radius * radius;
	double discriminant = b * b - a * c;

	if (discriminant > 0) {
		// get both solutions
		double t1 = (-b - std::sqrt(discriminant)) / a;
		double t2 = (-b + std::sqrt(discriminant)) / a;

		// check which one is inside
		bool t1inside = is_between(t1, tmin, tmax);
		bool t2inside = is_between(t2, tmin, tmax);

		// early return
		if (!t1inside && !t2inside) return false;

		// determine the right parameter t
		double t = t1;
		if (t1inside && t2inside) t = std::min(t1, t2);
		else if (t2inside)        t = t2;

		// set hit record
		rec.t = t;
		rec.p = r.position(rec.t);
		rec.normal = normalize(rec.p - center);
		rec.material = material;
		texture_coordinates(rec.normal, rec.u, rec.v);
		rec.lp = rec.p - center;

		return true;
	}

	return false;
};

bool rt::Sphere::boundingbox(aabb& box) const {
	box = aabb(center - vec3(radius), center + vec3(radius));
	return true;
}

void rt::Sphere::texture_coordinates(const vec3& p, float& u, float& v) const {
	// cartesian to spherical coordinates
	float r = rt::length(p);
	float theta = std::atan2f(p.x, p.z) + rt::PI;
	float phi = std::acosf(p.y / r);

	// spherical coordinates to uv space
	u = theta / rt::TWO_PI;
	v = phi / rt::PI;
}