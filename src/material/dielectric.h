#ifndef DIELECTRICT_H
#define DIELECTRICT_H

#include "imaterial.h"
#include "texture/itexture.h"
#include "texture/constanttexture.h"

namespace rt {
	class Dielectric : public IMaterial {
	public:
		Dielectric(float ri, std::shared_ptr<ITexture> texture = new_color(vec3(1)))
			: m_refractionidx(ri), m_texture(texture) {}

		virtual bool scatter(const ray& rIn, const HitRecord& rec, vec3& attenuation, ray& scattered) const {
			vec3 outwardNormal;
			vec3 reflected = reflect(rIn.dir, rec.normal);
			double niOverNt;
			attenuation = m_texture->value(rec.u, rec.v, rec.lp);
			double cosine;
			double reflectProb;
		
			vec3 refracted;
			if (dot(rIn.dir, rec.normal) > 0) {
				outwardNormal = -rec.normal;
				niOverNt = m_refractionidx;
				cosine = m_refractionidx * dot(rIn.dir, rec.normal) / rIn.dir.length();
			} else {
				outwardNormal = rec.normal;
				niOverNt = 1.0 / m_refractionidx;
				cosine = -dot(rIn.dir, rec.normal) / rIn.dir.length();
			}

			if (refract(rIn.dir, outwardNormal, niOverNt, refracted)) {
				reflectProb = schlick(cosine, m_refractionidx);
			} else {
				scattered = ray(rec.p, reflected);
				reflectProb = 1.0;
			}

			if (drand() < reflectProb) {
				scattered = ray(rec.p, reflected);
			} else {
				scattered = ray(rec.p, refracted);
			}

			return true;
		}

	private:
		float m_refractionidx;
		std::shared_ptr<ITexture> m_texture;
	};
}

#endif//DIELECTRICT_H