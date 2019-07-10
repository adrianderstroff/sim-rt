#ifndef TRANSLATION_H
#define TRANSLATION_H

#include "hitable/ihitable.h"

namespace rt {
	class Translation : public IHitable {
	public:
		Translation(std::shared_ptr<IHitable> hitable, const vec3& offset) : m_hitable(hitable), m_offset(offset) { }

		virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const override;
		virtual bool boundingbox(aabb& box) const override;

	private:
		std::shared_ptr<IHitable> m_hitable;
		vec3 m_offset;
	};

	bool Translation::hit(const ray& r, double tmin, double tmax, HitRecord& record) const {
		ray translatedray(r.o - m_offset, r.dir);
		if (m_hitable->hit(translatedray, tmin, tmax, record)) {
			record.p += m_offset;
			return true;
		}

		return false;
	}
	bool Translation::boundingbox(aabb& box) const {
		if (m_hitable->boundingbox(box)) {
			box = aabb(box.min() + m_offset, box.max() + m_offset);
			return true;
		}

		return false;
	}
}

#endif//TRANSLATION_H