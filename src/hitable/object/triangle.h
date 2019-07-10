#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hitable/ihitable.h"
#include "math/constants.h"
#include "math/vec3.h"

namespace rt {
	class Triangle : public IHitable {
	public:
		Triangle() {}
		Triangle(vec3 p1, vec3 p2, vec3 p3, std::shared_ptr<IMaterial> mat) 
			: p1(p1), p2(p2), p3(p3), t1(0), t2(0), t3(0), m_material(mat) {
			n1 = n2 = n3 = rt::normal(p1, p2, p3);
		};
		Triangle(vec3 p1, vec3 p2, vec3 p3, vec3 n1, vec3 n2, vec3 n3, std::shared_ptr<IMaterial> mat) 
			: p1(p1), p2(p2), p3(p3), n1(n1), n2(n2), n3(n3), t1(0), t2(0), t3(0), m_material(mat) { };
		Triangle(vec3 p1, vec3 p2, vec3 p3, vec3 n1, vec3 n2, vec3 n3, vec3 t1, vec3 t2, vec3 t3, std::shared_ptr<IMaterial> mat)
			: p1(p1), p2(p2), p3(p3), n1(n1), n2(n2), n3(n3), t1(t1), t2(t2), t3(t3), m_material(mat) { };

		virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const override;
		virtual bool boundingbox(aabb& box) const override;

		vec3 p1, p2, p3;
		vec3 n1, n2, n3;
		vec3 t1, t2, t3;

	private:
		std::shared_ptr<IMaterial> m_material;

	/**
	 * calculates the barycenteric coordinates of x relative to
	 * the triangle defined by p1,p2,p3
	 * @param x - point to determine barycentric coordinates for
	 * @param p1 - first point of the triangle
	 * @param p2 - second point of the triangle
	 * @param p3 - third point of the triangle
	 * @return barycentric coordinates of x
	 */
	 vec3 barycentric(const vec3& x, const vec3& p1, const vec3& p2, const vec3& p3) const;

	 /**
	  * calculates the position based on the barycentric coordinates
	  * and the three vectors p1,p2,p3
	  * @param b - barycentric coordinates (u,v,w)
	  * @param p1 - first vector
	  * @param p2 - second vector
	  * @param p3 - third vector
	  * @return vector reconstructed from the barycentric coordinates
	  * and the three vectors
	  */
	  vec3 from_barycentric(const vec3& b, const vec3& p1, const vec3& p2, const vec3& p3) const;
	};
}

#endif//TRIANGLE_H