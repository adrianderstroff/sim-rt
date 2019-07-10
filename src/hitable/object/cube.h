#ifndef CUBE_H
#define CUBE_H

#include "hitable/ihitable.h"
#include "hitable/organization/bvh.h"
#include "rectangle.h"

namespace rt {
	class Cube : public IHitable {
	public:
		Cube(vec3 pos, float width, float height, float depth, std::shared_ptr<IMaterial> mat, bool invert = false);

		virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const;
		virtual bool boundingbox(aabb& box) const;

	private:
		vec3 m_position;
		float m_width, m_height, m_depth;
		std::shared_ptr<BVH> m_rectangles;
		std::shared_ptr<IMaterial> m_material;

		void create_rectangles(bool invert);
	};
}

#endif//CUBE_H