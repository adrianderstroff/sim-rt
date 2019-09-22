#include "algorithm.h"

double rt::drand() {
	return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
}

double rt::schlick(double cosine, double refractionIdx) {
	double r0 = (1 - refractionIdx) / (1 + refractionIdx);
	r0 = r0 * r0;
	return r0 + (1 - r0)*pow(1 - cosine, 5);
}

bool rt::is_between(double val, double vmin, double vmax) {
	return vmin < val && val < vmax;
}

double rt::saturate(double val) {
	return std::min(std::max(val, 0.0), 1.0);
}