#include "cylinder.h"

rt::Cylinder::Cylinder(const vec3& p1, const vec3& p2, float r, std::shared_ptr<IMaterial> mat) 
	: m_p1(p1), m_p2(p2), m_radius(r), m_material(mat) {
	// determine cylinder coordinate system
	m_z = normalize(p2 - p1);
	m_y = (std::abs(dot(m_z, vec3(0, 1, 0))) > 1.0 - rt::EPS) ? vec3(0, 0, -1) : vec3(0, 1, 0);
	m_x = cross(m_y, m_z);
	m_y = cross(m_z, m_x);
};

bool rt::Cylinder::hit(const ray& r, double tmin, double tmax, HitRecord& rec) const {
	// determine ray in the coordinate system of the cylinder
	double ox = dot(r.o - m_p1, m_x);
	double oy = dot(r.o - m_p1, m_y);
	double oz = dot(r.o - m_p1, m_z);
	double dirx = dot(r.dir, m_x);
	double diry = dot(r.dir, m_y);
	double dirz = dot(r.dir, m_z);
	
	// solving the general quadratic formula
	double a = dirx*dirx + diry*diry;
	double b = ox*dirx + oy*diry;
	double c = ox*ox + oy*oy - m_radius * m_radius;
	double discriminant = b * b - a * c;

	if (discriminant > 0) {
		double t1 = (-b - std::sqrt(discriminant)) / a;
		double t2 = (-b + std::sqrt(discriminant)) / a;

		// range check
		bool t1inside = is_between(t1, tmin, tmax);
		bool t2inside = is_between(t2, tmin, tmax);

		// both intersections are outside
		if (!t1inside && !t2inside) return false;

		// calculate caps
		double z1 = oz + t1 * dirz;
		double z2 = oz + t2 * dirz;
		double zmin = 0;
		double zmax = dot(m_p2 - m_p1, m_z);
		double t3 = (zmin - oz) / dirz;
		double t4 = (zmax - oz) / dirz;
		double x3 = ox + t3 * dirx;
		double x4 = ox + t4 * dirx;
		double y3 = oy + t3 * diry;
		double y4 = oy + t4 * diry;
		double radius2 = m_radius * m_radius;

		// range check
		t1inside = t1inside && is_between(z1, zmin, zmax);
		t2inside = t2inside && is_between(z2, zmin, zmax);
		bool t3inside = is_between(t3, tmin, tmax) && std::abs(x3 * x3 + y3 * y3) < radius2;
		bool t4inside = is_between(t4, tmin, tmax) && std::abs(x4 * x4 + y4 * y4) < radius2;

		// determine the right parameter t
		double t = DBL_MAX;
		bool iscap = false;
		bool hit = false;
		if (t1inside && t1 < t) { t = t1; iscap = false; hit = true; }
		if (t2inside && t2 < t) { t = t2; iscap = false; hit = true; }
		if (t3inside && t3 < t) { t = t3; iscap = true; hit = true; }
		if (t4inside && t4 < t) { t = t4; iscap = true; hit = true; }

		if (!hit) return false;

		// calculate intersection information
		rec.t = t;
		rec.p = r.position(rec.t);
		vec3 pp = (iscap) ? m_p1 + 0.5*(m_p2 - m_p1) : m_p1 + dot(rec.p - m_p1, m_z) * m_z;
		rec.normal = normalize(rec.p - pp);
		rec.material = m_material;
		texture_coordinates(rec.p, rec.u, rec.v);
		rec.lp = rec.p - m_p1;
		return true;
	}

	return false;
};

bool rt::Cylinder::boundingbox(aabb& box) const {
	// http://www.iquilezles.org/www/articles/diskbbox/diskbbox.htm
	vec3 a = m_p2 - m_p1;
	vec3 e = m_radius * sqrt(1.0 - a * a / dot(a, a));
	box = aabb(min(m_p1 - e, m_p2 - e), max(m_p1 + e, m_p2 + e));
	return true;
}

void rt::Cylinder::texture_coordinates(const vec3& p, float& u, float& v) const {
	// cartesian to spherical coordinates
	double x = dot(p - m_p1, m_x);
	double y = dot(p - m_p1, m_y);
	double theta = std::atan2f(y, x) + rt::PI;

	// spherical coordinates to uv space
	u = theta / rt::TWO_PI;
	v = std::abs(dot(p - m_p2, m_z)) / length(m_p2 - m_p1);
}