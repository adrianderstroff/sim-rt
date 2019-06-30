#include "sphere.h"

bool rt::Sphere::hit(const ray& r, double tMin, double tMax, HitRecord& rec) const {
	vec3 oc = r.o - center;
	double a = dot(r.dir, r.dir);
	double b = dot(oc, r.dir);
	double c = dot(oc, oc) - radius * radius;
	double discriminant = b * b - a * c;

	if (discriminant > 0) {
		double temp = (-b - sqrt(b*b - a * c)) / a;
		if (temp < tMax && temp > tMin) {
			rec.t = temp;
			rec.p = r.position(rec.t);
			rec.normal = normalize(rec.p - center);
			rec.material = material;
			texture_coordinates(rec.p, rec.u, rec.v);
			return true;
		}
		temp = (-b + sqrt(b*b - a * c)) / a;
		if (temp < tMax && temp > tMin) {
			rec.t = temp;
			rec.p = r.position(rec.t);
			rec.normal = normalize(rec.p - center);
			rec.material = material;
			texture_coordinates(rec.p, rec.u, rec.v);
			return true;
		}
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
	float theta = std::atan2f(p.z, p.x) + constants::PI;
	float phi = std::acosf(p.y / r);

	// spherical coordinates to uv space
	u = theta / constants::TWO_PI;
	v = phi / constants::PI;
}