#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include "util.h"

class vec3 {
public:
	vec3() {};
    vec3(double e0, double e1, double e2) { e[0] = e0; e[1] = e1; e[2] = e2; }

    inline const vec3& operator+() const { return *this; }
    inline vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    inline double operator[](int i) const { return e[i]; }

    inline vec3& operator+=(const vec3 &v2);
    inline vec3& operator-=(const vec3 &v2);
    inline vec3& operator*=(const vec3 &v2);
    inline vec3& operator/=(const vec3 &v2);
    inline vec3& operator*=(const double t);
    inline vec3& operator/=(const double t);

    inline double length() const { return sqrt(e[0]*e[0]+e[1]*e[1]+e[2]*e[2]); }
    inline double length2() const { return e[0]*e[0]+e[1]*e[1]+e[2]*e[2]; }
    inline void normalize();

	union {
		double e[3];
		struct { double x, y, z; };
		struct { double r, g, b; };
		struct { double s, t, p; };
	};
};

inline vec3& vec3::operator+=(const vec3 &v) {
	e[0] += v.e[0];
	e[1] += v.e[1];
	e[2] += v.e[2];
	return *this;
}
inline vec3& vec3::operator-=(const vec3 &v) {
	e[0] -= v.e[0];
	e[1] -= v.e[1];
	e[2] -= v.e[2];
	return *this;
}
inline vec3& vec3::operator*=(const vec3 &v) {
	e[0] *= v.e[0];
	e[1] *= v.e[1];
	e[2] *= v.e[2];
	return *this;
}
inline vec3& vec3::operator/=(const vec3 &v) {
	e[0] /= v.e[0];
	e[1] /= v.e[1];
	e[2] /= v.e[2];
	return *this;
}
inline vec3& vec3::operator*=(const double t) {
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
	return *this;
}
inline vec3& vec3::operator/=(const double t) {
	e[0] /= t;
	e[1] /= t;
	e[2] /= t;
	return *this;
}

inline vec3 operator+(const vec3 &v1, const vec3 &v2) {
	return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}
inline vec3 operator-(const vec3 &v1, const vec3 &v2) {
	return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}
inline vec3 operator*(const vec3 &v1, const vec3 &v2) {
	return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}
inline vec3 operator/(const vec3 &v1, const vec3 &v2) {
	return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}
inline vec3 operator*(double t, const vec3 &v) {
	return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}
inline vec3 operator/(const vec3 &v, double t) {
	return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}
inline vec3 operator*(const vec3 &v, double t) {
	return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 normalize(const vec3 &v) {
	return v / v.length();
}
inline void vec3::normalize() {
	double norm = 1.0 / this->length();
	e[0] *= norm;
	e[1] *= norm;
	e[2] *= norm;
}

inline double dot(const vec3 &v1, const vec3 &v2) {
	return v1.e[0]*v2.e[0] + v1.e[1]*v2.e[1] + v1.e[2]*v2.e[2];
}
inline vec3 cross(const vec3 &v1, const vec3 &v2) {
	return vec3(
		(v1.e[1]*v2.e[2] - v1.e[2]*v2.e[1]),
		(-(v1.e[0]*v2.e[2] - v1.e[2]*v2.e[0])),
		(v1.e[0]*v2.e[1] - v1.e[1]*v2.e[0])
	);
}

inline vec3 randomDir() {
	vec3 p;
	do { p = 2.0*vec3(drand(), drand(), drand()) - vec3(1, 1, 1); } while (p.length2() >= 1.0);
	return p;
}
inline vec3 lerp(const vec3 &v1, const vec3 &v2, double t) {
	return (1.0 - t) * v1 + t * v2;
}
inline vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n)*n;
}
inline bool refract(const vec3& v, const vec3& n, double niOverNt, vec3& refracted) {
	vec3 uv = normalize(v);
	double dt = dot(uv, n);
	double discriminant = 1.0 - niOverNt * niOverNt*(1 - dt * dt);

	if (discriminant > 0) {
		refracted = niOverNt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	else return false;
}
inline std::ostream& operator<<(std::ostream& os, const vec3& v) {
	os << "(" << v.x << "," << v.y << "," << v.z << ")" << std::endl;
	return os;
}

#endif//VEC3_H