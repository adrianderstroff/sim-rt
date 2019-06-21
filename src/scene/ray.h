#ifndef RAY_H
#define RAY_H

#include "math/vec3.h"

class ray {
public:
    ray() {}
    ray(const vec3& o, const vec3& dir) : o(o), dir(dir) { }
    vec3 position(double t) const { return o + t*dir; }

    vec3 o;
    vec3 dir;
};

#endif//RAY_H