#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"
#include "math/constants.h"

namespace rt {
	class Camera {
	public:
		Camera() { }
		Camera(vec3 pos, vec3 lookAt, vec3 up, double verticalFov, double aspectRatio) {
			m_pos = pos;
			m_up = up;
			m_lookat = lookAt;
			m_aspectratio = aspectRatio;
		
			double theta = verticalFov * rt::DEG_TO_RAD;
			m_halfheight = tan(theta / 2);
			m_halfwidth = aspectRatio * m_halfheight;

			calculate_coordinate_system();
		}

		/**
		 * get a ray for the position (s,t) that starts in the upper left corner
		 * and continues right and down with increasing s and t
		 */
		ray getRay(double s, double t) {
			return ray(m_origin, m_upperleftcorner + s*m_horizontal - t*m_vertical - m_origin);
		}

		void setPosition(const vec3& pos) { 
			m_pos = pos;
			calculate_coordinate_system();
		}
		void setLookat(const vec3& lookat) {
			m_lookat = lookat;
			calculate_coordinate_system();
		}

	private:
		// input members
		vec3 m_pos, m_lookat, m_up;
		float m_halfwidth, m_halfheight, m_aspectratio;
		// image plane coordinate system
		vec3 m_upperleftcorner;
		vec3 m_horizontal;
		vec3 m_vertical;
		vec3 m_origin;
		// camera coordinate system
		vec3 m_u, m_v, m_w;

		void calculate_coordinate_system() {
			m_w = normalize(m_pos - m_lookat);
			m_u = normalize(cross(m_up, m_w));
			m_v = cross(m_w, m_u);

			m_origin = m_pos;
			m_upperleftcorner = m_origin - m_halfwidth * m_u + m_halfheight * m_v - m_w;
			m_horizontal = 2 * m_halfwidth  * m_u;
			m_vertical = 2 * m_halfheight * m_v;
		}
	};
}

#endif//CAMERA_H