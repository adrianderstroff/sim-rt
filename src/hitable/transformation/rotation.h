#ifndef ROTATION_H
#define ROTATION_H

#include "hitable/ihitable.h"

namespace rt {
	class Rotation : public IHitable {
	public:
		Rotation(std::shared_ptr<IHitable> hitable, const vec3& axis, float angle);

		virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const override;
		virtual bool boundingbox(aabb& box) const override;

	private:
		std::shared_ptr<IHitable> m_hitable;
		vec3 m_axis;
		float m_theta;
		aabb m_bounds;
		bool m_hasbounds;

		vec3 rotate(const vec3& v, float theta) const;
	};
}

#endif//ROTATION_H