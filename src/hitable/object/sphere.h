#ifndef SPHERE_H
#define SPHERE_H

#include "hitable/ihitable.h"
#include "io/console.h"
#include "math/vec3.h"

namespace rt {
	class Sphere: public IHitable {
	public:
		Sphere() : radius(0) {}
		Sphere(vec3 c, float r, std::shared_ptr<IMaterial> mat) : center(c), radius(r), material(mat) {};
    
		virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const;
		virtual bool boundingbox(aabb& box) const;

		vec3 center;
		double radius;
		std::shared_ptr<IMaterial> material;

	private:
		void texture_coordinates(const vec3& p, float& u, float& v) const;
	};
}

#endif//SPHERE_H