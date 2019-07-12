#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hitable/ihitable.h"
#include "material/isotropic.h"
#include "texture/itexture.h"

namespace rt {
	class ConstantMedium : public IHitable {
	public:
		ConstantMedium(std::shared_ptr<IHitable> hitable, float density, std::shared_ptr<ITexture> texture);

		virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const;
		virtual bool boundingbox(aabb& box) const;

	private:
		std::shared_ptr<IHitable> m_boundary;
		float m_density;
		std::shared_ptr<IMaterial> m_phasefunction;
	};
}

#endif//CONSTANT_MEDIUM_H