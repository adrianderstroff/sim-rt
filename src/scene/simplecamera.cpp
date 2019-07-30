#include "simplecamera.h"

rt::SimpleCamera::SimpleCamera() { }
rt::SimpleCamera::SimpleCamera(vec3 pos, vec3 lookAt, vec3 up, double verticalFov, double aspectRatio) {
	m_pos = pos;
	m_up = up;
	m_lookat = lookAt;
	m_aspectratio = aspectRatio;

	double theta = verticalFov * rt::DEG_TO_RAD;
	m_halfheight = tan(theta / 2);
	m_halfwidth = aspectRatio * m_halfheight;

	calculate_coordinate_system();
}

rt::ray rt::SimpleCamera::get_ray(double s, double t) {
	return ray(m_origin, m_upperleftcorner + s * m_horizontal + t * m_vertical - m_origin);
}

rt::vec3  rt::SimpleCamera::get_position() { return m_origin; }
void rt::SimpleCamera::set_position(const vec3& pos) {
	m_pos = pos;
	calculate_coordinate_system();
}
void rt::SimpleCamera::set_lookat(const vec3& lookat) {
	m_lookat = lookat;
	calculate_coordinate_system();
}

rt::vec3  rt::SimpleCamera::get_image_plane_origin() { return m_upperleftcorner; }
rt::vec3  rt::SimpleCamera::get_image_plane_xaxis()  { return m_horizontal;      }
rt::vec3  rt::SimpleCamera::get_image_plane_yaxis()  { return m_vertical;        }
float     rt::SimpleCamera::get_image_plane_width()  { return 2 * m_halfwidth;   }
float     rt::SimpleCamera::get_image_plane_height() { return 2 * m_halfheight;  }

void rt::SimpleCamera::calculate_coordinate_system() {
	m_w = normalize(m_pos - m_lookat);
	m_u = normalize(cross(m_up, m_w));
	m_v = cross(m_w, m_u);

	m_origin = m_pos;
	m_upperleftcorner = m_origin - m_halfwidth * m_u + m_halfheight * m_v - m_w;
	m_horizontal = 2.0 * m_halfwidth * m_u;
	m_vertical = -2.0 * m_halfheight * m_v;
}