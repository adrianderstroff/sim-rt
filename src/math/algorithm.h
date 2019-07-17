#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <random>

#include "constants.h"

namespace rt {
	/**
	 * returns a random floating point number in the range [0,1)
	 * @return random number
	 */
	double drand();

	/**
	 * Schlick's approximation of the Fresnel Equation 
	 * @param cosine - angle of the incident ray and the surface normal
	 * @param refractionIdx - refraction index of the entered medium
	 * @return specular reflection coefficient
	 */
	double schlick(double cosine, double refractionIdx);

	/**
	 * checks if val is in the range ]vmin, vmax[
	 * @param val - the value to test for
	 * @param vmin - the lower bound
	 * @param vmax - the upper bound
	 * @return true if val is in the range ]vmin, vmax[
	 * and false otherwise
	 */
	bool is_between(double val, double vmin, double vmax);
}

#endif//ALGORITHM_H