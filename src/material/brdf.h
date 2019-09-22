#ifndef BRDF_H
#define BRDF_H

#include "imaterial.h"
#include "math/constants.h"
#include "texture/itexture.h"

namespace rt {
	class BRDF : public IMaterial {
	public:
		BRDF(std::shared_ptr<ITexture> a, double metalness, double kd, double roughness)
			: m_albedo(a), m_metalness(metalness), m_kd(kd), m_roughness(roughness) {}

		virtual bool scatter(const ray& rin, const HitRecord& rec, vec3& attenuation, ray& scattered) const {
			// cook-torrance brdf

			// get surface color
			vec3 surfacecolor = m_albedo->value(rec.u, rec.v, rec.lp);
			vec3 f0 = lerp(vec3(0.04), surfacecolor, m_metalness);

			// generate random number to determine wether we
			// perform a diffuse reflection or aspecular reflection
			double r = drand();

			// diffuse component
			if (r <= m_kd) {
				// lambertian reflection
				vec3 target = rec.p + rec.normal + randomDir();
				vec3 dir = normalize(target - rec.p);
				scattered = ray(rec.p, dir);

				attenuation = m_kd * surfacecolor / PI;
			}
			// specular component
			else {
				// mirror reflection
				vec3 reflected = reflect(normalize(rin.dir), normalize(rec.normal));
				scattered = ray(rec.p, reflected);

				// grab all relevant vectors and the roughness
				double a = m_roughness;
				vec3 n = normalize(rec.normal);
				vec3 l = normalize(-rin.dir);
				vec3 v = normalize(reflected);
				vec3 h = normalize(l + v);

				// mapping between roughness and parameter k
				// for the fresnel schlick approximation
				double k = (a * a) / 2.0;

				// compute dot products
				double ndotl = saturate(dot(n, l));
				double ndoth = saturate(dot(n, h));
				double ldoth = saturate(dot(l, h));
				double ndotv = saturate(dot(n, v));

				// calculate brdf
				double d = normal_distribution_ggx(ndoth, a);
				double g = geometry_smith(ndotl, ndotv, k);
				vec3 f = fresnel_schlick(ldoth, f0);
				vec3 ggx = (d * f * g) / (4 * ndotl * ndotv);

				double prob = d * ndoth / (4 * ldoth);

				// accumulate
				attenuation = ggx / (prob * (1.0 - m_kd));
				attenuation = saturate(attenuation);
			}

			return true;
		}

	private:
		std::shared_ptr<ITexture> m_albedo;
		double m_metalness;
		double m_kd;
		double m_roughness;

		/**
		 * trowbridge-reitz ggx normal distribution function
		 * approximates the relative surface area of microfacets, that are
		 * exactly aligned with the halfway vector h. The parameter a 
		 * specifies the roughness of the surface.
		 */
		double normal_distribution_ggx(double ndoth, double a) const {
			double a2 = a * a;
			double d = (ndoth * (a2*ndoth - ndoth) + 1.0);

			return a2 / (PI * d * d);
		}


		/**
		 * schlick approximation of the smith equation
		 */
		double geometry_schlick_ggx(double ndotv, double k) const {
			return ndotv / (ndotv * (1.0 - k) + k);
		}

		/**
		 * specifies the geometric shadowing of the microfacets based
		 * on the view vector and the roughness of the surface. here
		 * k depends on the roughness of the surface. this implementation
		 * uses the smith method.
		 */
		double geometry_smith(double ndotl, double ndotv, double k) const {
			double ggx1 = geometry_schlick_ggx(ndotv, k);
			double ggx2 = geometry_schlick_ggx(ndotl, k);

			return ggx1 * ggx2;
		}

		/**
		 * specifies the reflection of light on a smooth surface
		 */
		vec3 fresnel_schlick(double ldoth, vec3 f0) const {
			return f0 + (vec3(1.0) - f0) * std::pow(1.0 - ldoth, 5.0);
		}
	};
}

#endif BRDF_H