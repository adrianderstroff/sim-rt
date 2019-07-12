#include "constantmedium.h"

rt::ConstantMedium::ConstantMedium(std::shared_ptr<IHitable> hitable, float density, std::shared_ptr<ITexture> texture)
	: m_boundary(hitable), m_density(density) {
	m_phasefunction = std::make_shared<Isotropic>(texture);
}

bool rt::ConstantMedium::hit(const ray& r, double tmin, double tmax, HitRecord& rec) const {
	HitRecord record1, record2;
	if (m_boundary->hit(r, -FLT_MAX, FLT_MAX, record1)) {
		if (m_boundary->hit(r, record1.t+1e-4, FLT_MAX, record2)) {
			// clamp parameter t to the bounds of the object
			record1.t = std::max(record1.t, tmin);
			record2.t = std::min(record2.t, tmax);
			if (record1.t >= record2.t) return false;
			record1.t = std::max(record1.t, 0.0);

			// determine probability of a hit
			float distance = (record2.t - record1.t) * length(r.dir);
			float hitdistance = -(1.f / m_density) * std::log(drand());
			if (hitdistance < distance) {
				rec.t = record1.t + hitdistance / length(r.dir);
				rec.p = r.position(rec.t);
				rec.lp = record1.lp;
				rec.normal = vec3(1, 0, 0); // doesn't matter
				rec.material = m_phasefunction;
				return true;
			}
		}
	}

	return false;
}
bool rt::ConstantMedium::boundingbox(aabb& box) const {
	aabb bounds;
	if (m_boundary->boundingbox(bounds)) {
		box = bounds;
		return true;
	}

	return false;
}