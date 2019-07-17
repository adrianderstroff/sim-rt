#include "vec3.h"

//-----------------------------------------------------------------------------//
// unary member operators                                                      //
//-----------------------------------------------------------------------------//

rt::vec3& rt::vec3::operator+=(const vec3& v) {
	e[0] += v.e[0];
	e[1] += v.e[1];
	e[2] += v.e[2];
	return *this;
}
rt::vec3& rt::vec3::operator-=(const vec3& v) {
	e[0] -= v.e[0];
	e[1] -= v.e[1];
	e[2] -= v.e[2];
	return *this;
}
rt::vec3& rt::vec3::operator*=(const vec3& v) {
	e[0] *= v.e[0];
	e[1] *= v.e[1];
	e[2] *= v.e[2];
	return *this;
}
rt::vec3& rt::vec3::operator/=(const vec3& v) {
	e[0] /= v.e[0];
	e[1] /= v.e[1];
	e[2] /= v.e[2];
	return *this;
}
rt::vec3& rt::vec3::operator*=(const double t) {
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
	return *this;
}
rt::vec3& rt::vec3::operator/=(const double t) {
	e[0] /= t;
	e[1] /= t;
	e[2] /= t;
	return *this;
}

//-----------------------------------------------------------------------------//
// geometric member functions                                                  //
//-----------------------------------------------------------------------------//

double rt::vec3::length() const {
	return std::sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); 
}
double rt::vec3::length2() const {
	return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; 
}
void rt::vec3::normalize() {
	double norm = 1.0 / this->length();
	e[0] *= norm;
	e[1] *= norm;
	e[2] *= norm;
}

//-----------------------------------------------------------------------------//
// binary operators                                                            //
//-----------------------------------------------------------------------------//

rt::vec3 rt::operator+(const rt::vec3 &v1, const rt::vec3 &v2) {
	return rt::vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}
rt::vec3 rt::operator-(const rt::vec3 &v1, const rt::vec3 &v2) {
	return rt::vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}
rt::vec3 rt::operator*(const rt::vec3 &v1, const rt::vec3 &v2) {
	return rt::vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}
rt::vec3 rt::operator/(const rt::vec3 &v1, const rt::vec3 &v2) {
	return rt::vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}
rt::vec3 rt::operator*(double t, const rt::vec3 &v) {
	return rt::vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}
rt::vec3 rt::operator/(const rt::vec3 &v, double t) {
	return rt::vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}
rt::vec3 rt::operator*(const rt::vec3 &v, double t) {
	return rt::vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

//-----------------------------------------------------------------------------//
// other operators                                                             //
//-----------------------------------------------------------------------------//

std::ostream& rt::operator<<(std::ostream& os, const rt::vec3& v) {
	os << "(" << v.x << "," << v.y << "," << v.z << ")" << std::endl;
	return os;
}

//-----------------------------------------------------------------------------//
// geometric functions                                                         //
//-----------------------------------------------------------------------------//

double rt::dot(const rt::vec3 &v1, const rt::vec3 &v2) {
	return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}
rt::vec3 rt::cross(const rt::vec3 &v1, const rt::vec3 &v2) {
	return rt::vec3(
		(v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
		(-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
		(v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0])
	);
}
float rt::length(const rt::vec3& v) {
	return v.length();
}
rt::vec3 rt::normalize(const rt::vec3 &v) {
	return v / v.length();
}
rt::vec3 rt::normal(const rt::vec3& p1, const rt::vec3& p2, const rt::vec3& p3) {
	rt::vec3 v1 = normalize(p2 - p1);
	rt::vec3 v2 = normalize(p3 - p1);
	return rt::cross(v1, v2);
}
rt::vec3 rt::randomDir() {
	rt::vec3 p;
	do { p = 2.0*rt::vec3(drand(), drand(), drand()) - rt::vec3(1, 1, 1); } while (p.length2() >= 1.0);
	return p;
}
rt::vec3 rt::reflect(const rt::vec3& v, const rt::vec3& n) {
	return v - 2 * rt::dot(v, n)*n;
}
bool rt::refract(const rt::vec3& v, const rt::vec3& n, double niOverNt, rt::vec3& refracted) {
	rt::vec3 uv = rt::normalize(v);
	double dt = rt::dot(uv, n);
	double discriminant = 1.0 - niOverNt * niOverNt*(1 - dt * dt);

	if (discriminant > 0) {
		refracted = niOverNt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	else return false;
}

//-----------------------------------------------------------------------------//
// math functions                                                              //
//-----------------------------------------------------------------------------//

rt::vec3 rt::lerp(const rt::vec3& v1, const rt::vec3& v2, double t) {
	return (1.0 - t) * v1 + t * v2;
}
rt::vec3 rt::min(const rt::vec3& v1, const rt::vec3& v2) {
	float x = std::min(v1.x, v2.x);
	float y = std::min(v1.y, v2.y);
	float z = std::min(v1.z, v2.z);
	return rt::vec3(x, y, z);
}
rt::vec3 rt::max(const rt::vec3& v1, const rt::vec3& v2) {
	float x = std::max(v1.x, v2.x);
	float y = std::max(v1.y, v2.y);
	float z = std::max(v1.z, v2.z);
	return rt::vec3(x, y, z);
}
rt::vec3 rt::sqrt(const vec3& v) {
	return vec3(std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z));
}