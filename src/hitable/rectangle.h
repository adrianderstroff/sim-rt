#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "hitable.h"
#include "triangle.h"
#include "math/constants.h"


namespace rt {
	class Rectangle : public IHitable {
	public:
		Rectangle() {}
		Rectangle(vec3 p, float width, float height, std::shared_ptr<IMaterial> mat);
		Rectangle(vec3 p, vec3 right, vec3 up, std::shared_ptr<IMaterial> mat);

		virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const override;
		virtual bool boundingbox(aabb& box) const override;

	private:
		Triangle m_t1, m_t2;
	};
}

#endif//RECTANGLE_H