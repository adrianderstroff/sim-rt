#ifndef TRANSLATION_H
#define TRANSLATION_H

#include "hitable/ihitable.h"

namespace rt {
	class Translation : public IHitable {
	public:
		Translation(std::shared_ptr<IHitable> hitable, const vec3& offset);

		virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const override;
		virtual bool boundingbox(aabb& box) const override;

	private:
		std::shared_ptr<IHitable> m_hitable;
		vec3 m_offset;
	};
}

#endif//TRANSLATION_H