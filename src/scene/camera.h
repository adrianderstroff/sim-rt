#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"
#include "math/constants.h"

namespace rt {
class Camera {
public:
	Camera() { }
    Camera(vec3 pos, vec3 lookAt, vec3 up, double verticalFov, double aspectRatio) {
		double theta = verticalFov * constants::DEG_TO_RAD;
		double halfheight = tan(theta / 2);
		double halfwidth = aspectRatio * halfheight;

		m_w = normalize(pos - lookAt);
		m_u = normalize(cross(up, m_w));
		m_v = cross(m_w, m_u);

		m_origin = pos;
		m_lowerleftcorner = m_origin - halfwidth* m_u - halfheight* m_v - m_w;
		m_horizontal = 2 * halfwidth  * m_u;
		m_vertical   = 2 * halfheight * m_v;
    }

    ray getRay(double s, double t) {
        return ray(m_origin, m_lowerleftcorner + s*m_horizontal + t*m_vertical - m_origin);
    }

private:
    vec3 m_lowerleftcorner;
    vec3 m_horizontal;
    vec3 m_vertical;
    vec3 m_origin;
	vec3 m_u, m_v, m_w;
};
}

#endif//CAMERA_H