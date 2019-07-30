#ifndef DOF_CAMERA_H
#define DOF_CAMERA_H

#include "icamera.h"
#include "math/constants.h"

namespace rt {
	/**
	 * camera simulating depth of field
	 */
	class DOFCamera : public ICamera {
	public:
		DOFCamera();
		DOFCamera(vec3 pos, vec3 lookAt, vec3 up, double verticalFov, double aspectRatio, double aperture);

		/**
		 * get a ray for the position (s,t) that starts in the upper left corner
		 * and continues right and down with increasing s and t
		 */
		ray get_ray(double s, double t) override;

		// camera position and orientation
		vec3  get_position() override;
		void set_position(const vec3& pos) override;
		void set_lookat(const vec3& lookat) override;

		// camera coordinate system
		vec3  get_image_plane_origin() override;
		vec3  get_image_plane_xaxis()  override;
		vec3  get_image_plane_yaxis()  override;
		float get_image_plane_width()  override;
		float get_image_plane_height() override;

	private:
		// input members
		vec3 m_pos, m_lookat, m_up;
		float m_halfwidth, m_halfheight, m_aspectratio;
		// thin lens members
		double m_lensradius;
		double m_focusdist;
		// image plane coordinate system
		vec3 m_upperleftcorner;
		vec3 m_horizontal;
		vec3 m_vertical;
		vec3 m_origin;
		// camera coordinate system
		vec3 m_u, m_v, m_w;

		/**
		 * helper function for setting up the camera coordinate system
		 */
		void calculate_coordinate_system();

		/**
		 * returns a random position on a disk
		 */
		vec3 disk_sampling();
	};
}

#endif//DOF_CAMERA_H