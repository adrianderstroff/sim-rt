#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include "algorithm.h"

namespace rt {
class vec3 {
public:
	vec3() {};
	vec3(double e012) { e[0] = e012; e[1] = e012; e[2] = e012; }
    vec3(double e0, double e1, double e2) { e[0] = e0; e[1] = e1; e[2] = e2; }

    const vec3& operator+()       const { return *this;                     }
    vec3        operator-()       const { return vec3(-e[0], -e[1], -e[2]); }
    double      operator[](int i) const { return e[i];                      }

    vec3& operator+=(const vec3&  v);
    vec3& operator-=(const vec3&  v);
    vec3& operator*=(const vec3&  v);
    vec3& operator/=(const vec3&  v);
    vec3& operator*=(const double t);
    vec3& operator/=(const double t);

	double length() const;
	double length2() const;
	void normalize();

	union {
		double e[3];
		struct { double x, y, z; };
		struct { double r, g, b; };
		struct { double s, t, p; };
	};
};

vec3 operator+(const vec3& v1, const vec3& v2);
vec3 operator-(const vec3& v1, const vec3& v2);
vec3 operator*(const vec3& v1, const vec3& v2);
vec3 operator/(const vec3& v1, const vec3& v2);
vec3 operator*(double       t, const vec3&  v);
vec3 operator/(const vec3&  v, double       t);
vec3 operator*(const vec3&  v, double       t);

std::ostream& operator<<(std::ostream& os, const vec3& v);

vec3 randomDir();
float length(const vec3& v);
vec3 normalize(const vec3 &v);
vec3 normal(const vec3& p1, const vec3& p2, const vec3& p3);
double dot(const vec3 &v1, const vec3 &v2);
vec3 cross(const vec3 &v1, const vec3 &v2);
vec3 reflect(const vec3& v, const vec3& n);
bool refract(const vec3& v, const vec3& n, double niOverNt, vec3& refracted);

vec3 lerp(const vec3& v1, const vec3& v2, double t);
vec3 min(const vec3& v1, const vec3& v2);
vec3 max(const vec3& v1, const vec3& v2);
}

#endif//VEC3_H