#ifndef SPHERE_H
#define SPHERE_H

#include "hitable.h"

class Sphere: public Hitable {
public:
    Sphere() {}
    Sphere(vec3 c, float r, std::shared_ptr<Material> mat) : center(c), radius(r), material(mat) {};
    
    virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const;

    vec3 center;
    double radius;
	std::shared_ptr<Material> material;
};

bool Sphere::hit(const ray& r, double tMin, double tMax, HitRecord& rec) const {
    vec3 oc = r.o - center;
    double a = dot(r.dir, r.dir);
    double b = dot(oc, r.dir);
    double c = dot(oc, oc) - radius*radius;
    double discriminant = b*b - a*c;

    if(discriminant > 0) {
        double temp = (-b - sqrt(b*b - a*c)) / a;
        if(temp < tMax && temp > tMin) {
            rec.t = temp;
            rec.p = r.position(rec.t);
            rec.normal = (rec.p - center) / radius;
			rec.material = material;
            return true;
        }
        temp = (-b + sqrt(b*b - a*c)) / a;
        if(temp < tMax && temp > tMin) {
            rec.t = temp;
            rec.p = r.position(rec.t);
            rec.normal = (rec.p - center) / radius;
			rec.material = material;
            return true;
        }
    }

    return false;
};

#endif//SPHERE_H