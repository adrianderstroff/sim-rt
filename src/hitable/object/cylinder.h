#ifndef CYLINDER_H
#define CYLINDER_H

#include "hitable/ihitable.h"
#include "io/console.h"
#include "math/vec3.h"

namespace rt {
	class Cylinder : public IHitable {
	public:
		Cylinder() : m_radius(0) {}
		Cylinder(const vec3& p1, const vec3& p2, float r, std::shared_ptr<IMaterial> mat);

		virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const;
		virtual bool boundingbox(aabb& box) const;

	private:
		void texture_coordinates(const vec3& p, float& u, float& v) const;

		vec3 m_p1, m_p2;
		vec3 m_x, m_y, m_z;
		double m_radius;
		std::shared_ptr<IMaterial> m_material;
	};
}

#endif//CYLINDER_H