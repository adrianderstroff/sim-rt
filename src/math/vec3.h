#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <iostream>

#include "algorithm.h"

namespace rt {
	class vec3 {
	public:
		/**
		 * empty vector is initialized as zero vector (0,0,0)
		 */
		vec3() {};
		/**
		 * constructor with single value initialized for all 
		 * components (e012,e012,e012)
		 * @param e012 - value used for all three parameters
		 */
		vec3(double e012) { e[0] = e012; e[1] = e012; e[2] = e012; }
		/**
		 * constructor with three parameters
		 * @param e0 - first component
		 * @param e1 - first component
		 * @param e2 - first component
		 */
		vec3(double e0, double e1, double e2) { e[0] = e0; e[1] = e1; e[2] = e2; }

		/**
		 * + operator before vector does nothing
		 */
		const vec3& operator+() const { return *this; }
		/**
		 * - operator before vector inverts its components
		 */
		vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
		/**
		 * [] operator for accessing components
		 */
		double operator[](int i) const { return e[i]; }

		/**
		 * handy operators for vector math
		 */
		vec3& operator+=(const vec3&  v);
		vec3& operator-=(const vec3&  v);
		vec3& operator*=(const vec3&  v);
		vec3& operator/=(const vec3&  v);
		vec3& operator*=(const double t);
		vec3& operator/=(const double t);

		/**
		 * calculates the euclidean norm sqrt(x^2+y^2+z^2)
		 * @return euclidean norm
		 */
		double length() const;
		/**
		 * calculates the squared euclidean norm ||x^2+y^2+z^2||
		 * @return squared euclidean norm
		 */
		double length2() const;
		/**
		 * normalizes the length of the vector to size 1
		 */
		void normalize();

		/**
		 * union hack to make it possible to consistently
		 * update vector members
		 */
		union {
			double e[3];
			struct { double x, y, z; };
			struct { double r, g, b; };
			struct { double s, t, p; };
		};
	};

	/**
	 * handy binary operators for vector math
	 */
	vec3 operator+(const vec3& v1, const vec3& v2);
	vec3 operator-(const vec3& v1, const vec3& v2);
	vec3 operator*(const vec3& v1, const vec3& v2);
	vec3 operator/(const vec3& v1, const vec3& v2);
	vec3 operator*(double       t, const vec3&  v);
	vec3 operator/(const vec3&  v, double       t);
	vec3 operator*(const vec3&  v, double       t);

	/**
	 * add support for printing the vector
	 */
	std::ostream& operator<<(std::ostream& os, const vec3& v);

	/**
	 * return a unit vector with a random directory
	 */
	vec3 randomDir();
	/**
	 * calculates the euclidean norm sqrt(x^2+y^2+z^2)
	 * @return euclidean norm
	 */
	float length(const vec3& v);
	/**
	 * normalizes the length of the vector to size 1
	 */
	vec3 normalize(const vec3 &v);
	/**
	 * calculates a normal from three points
	 */
	vec3 normal(const vec3& p1, const vec3& p2, const vec3& p3);
	/**
	 * performs a dot product between two vectors v1 and v2
	 */
	double dot(const vec3 &v1, const vec3 &v2);
	/**
	 * performs a cross product between two vectors v1 and v2
	 */
	vec3 cross(const vec3 &v1, const vec3 &v2);
	/**
	 * reflects a vector v with respect to normal n
	 * the vector v has to point towards the normal
	 */
	vec3 reflect(const vec3& v, const vec3& n);
	/**
	 * refracts a vector v with respect to normal n
	 * the vector v has to point towards the normal
	 * niOverNt is the refraction coefficient
	 * and refract is the result of refraction
	 */
	bool refract(const vec3& v, const vec3& n, double niOverNt, vec3& refracted);

	/**
	 * performs a linear interpolation between two vectors v1 and v2
	 * with regards to t with the formula (1-t)*v1 + t*v2
	 */
	vec3 lerp(const vec3& v1, const vec3& v2, double t);
	/** 
	 * returns the componentwise minima of two vectors v1 and v2
	 */
	vec3 min(const vec3& v1, const vec3& v2);
	/**
	 * returns the componentwise maxima of two vectors v1 and v2
	 */
	vec3 max(const vec3& v1, const vec3& v2);
	/**
	 * performs a component wise square root
	 * with regards to t with the formula (1-t)*v1 + t*v2
	 */
	vec3 sqrt(const vec3& v);
}

#endif//VEC3_H