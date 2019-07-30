#ifndef I_CAMERA_H
#define I_CAMERA_H

#include "ray.h"

namespace rt {
	/**
	 * interface for all camera objects
	 */
	class ICamera {
	public:
		/**
		 * get a ray for the position (s,t) that starts in the upper left corner
		 * and continues right and down with increasing s and t
		 */
		virtual ray get_ray(double s, double t) = 0;

		// position and orientation
		virtual vec3  get_position() = 0;
		virtual void set_position(const vec3& pos) = 0;
		virtual void set_lookat(const vec3& lookat) = 0;

		// camera coordinate system
		virtual vec3  get_image_plane_origin() = 0;
		virtual vec3  get_image_plane_xaxis() = 0;
		virtual vec3  get_image_plane_yaxis() = 0;
		virtual float get_image_plane_width() = 0;
		virtual float get_image_plane_height() = 0;
	};
}

#endif//I_CAMERA_H