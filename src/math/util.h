#ifndef UTIL_H
#define UTIL_H

#include <random>

inline double drand() {
    return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
}

inline double schlick(double cosine, double refractionIdx) {
	double r0 = (1 - refractionIdx) / (1 + refractionIdx);
	r0 = r0 * r0;
	return r0 + (1 - r0)*pow(1 - cosine, 5);
}

#endif//UTIL_H