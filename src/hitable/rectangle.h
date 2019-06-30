#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "hitable.h"
#include "triangle.h"
#include "math/constants.h"


namespace rt {
	class Rectangle : public IHitable {
	public:
		Rectangle() {}
		Rectangle(vec3 p, float width, float height, std::shared_ptr<IMaterial> mat) {
			vec3 halfwidth = vec3(width/2, 0, 0);
			vec3 halfheight = vec3(0, height / 2, 0);

			// determine positions
			vec3 p1 = p - halfwidth + halfheight;
			vec3 p2 = p - halfwidth - halfheight;
			vec3 p3 = p + halfwidth + halfheight;
			vec3 p4 = p + halfwidth - halfheight;

			// determine normal
			vec3 n = rt::normal(p1, p2, p3);

			// determine texture coordinates
			vec3 t1(0, 1, 0);
			vec3 t2(0, 0, 0);
			vec3 t3(1, 1, 0);
			vec3 t4(1, 0, 0);

			m_t1 = Triangle(p1, p2, p3, n, n, n, t1, t2, t3, mat);
			m_t2 = Triangle(p3, p2, p4, n, n, n, t3, t2, t4, mat);
		};

		virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const override;
		virtual bool boundingbox(aabb& box) const override;

	private:
		Triangle m_t1, m_t2;
	};
}

#endif//RECTANGLE_H