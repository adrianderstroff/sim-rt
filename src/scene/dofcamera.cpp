#include "dofcamera.h"

rt::DOFCamera::DOFCamera() { }
rt::DOFCamera::DOFCamera(vec3 pos, vec3 lookAt, vec3 up, double verticalFov, double aspectRatio, double aperture) {
	// calculate size of the thin lens
	m_lensradius = aperture / 2.0;
	m_focusdist = length(lookAt - pos);
	
	// set members
	m_pos = pos;
	m_up = up;
	m_lookat = lookAt;
	m_aspectratio = aspectRatio;

	// calculate image plane size
	double theta = verticalFov * rt::DEG_TO_RAD;
	m_halfheight = tan(theta / 2);
	m_halfwidth = aspectRatio * m_halfheight;

	// calulate the camera coordinate system
	calculate_coordinate_system();
}

rt::ray rt::DOFCamera::get_ray(double s, double t) {
	vec3 rd = m_lensradius * disk_sampling();
	vec3 offset(s * rd.x, t * rd.y, 0);
	return ray(m_origin + offset, m_upperleftcorner + s * m_horizontal + t * m_vertical - m_origin - offset);
}

rt::vec3  rt::DOFCamera::get_position() { return m_origin; }
void rt::DOFCamera::set_position(const vec3& pos) {
	m_pos = pos;
	calculate_coordinate_system();
}
void rt::DOFCamera::set_lookat(const vec3& lookat) {
	m_lookat = lookat;
	calculate_coordinate_system();
}

rt::vec3  rt::DOFCamera::get_image_plane_origin() { return m_upperleftcorner; }
rt::vec3  rt::DOFCamera::get_image_plane_xaxis()  { return m_horizontal;      }
rt::vec3  rt::DOFCamera::get_image_plane_yaxis()  { return m_vertical;        }
float     rt::DOFCamera::get_image_plane_width()  { return 2 * m_halfwidth;   }
float     rt::DOFCamera::get_image_plane_height() { return 2 * m_halfheight;  }

void rt::DOFCamera::calculate_coordinate_system() {
	m_w = normalize(m_pos - m_lookat);
	m_u = normalize(cross(m_up, m_w));
	m_v = cross(m_w, m_u);

	m_origin = m_pos;
	m_upperleftcorner = m_origin - m_halfwidth * m_focusdist * m_u + m_halfheight * m_focusdist * m_v - m_focusdist * m_w;
	m_horizontal = 2.0 * m_halfwidth * m_focusdist * m_u;
	m_vertical = -2.0 * m_halfheight * m_focusdist * m_v;
}

rt::vec3 rt::DOFCamera::disk_sampling() {
	vec3 p;
	do {
		p = 2.0 * vec3(drand(), drand(), 0) - vec3(1, 1, 0);
	} while (dot(p, p) >= 1.0);
	return p;
}