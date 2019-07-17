#ifndef I_TRACER_H
#define I_TRACER_H

#include <string>
#include <memory>
#include <tuple>

#include "hitable/ihitable.h"
#include "scene/camera.h"
#include "math/vec3.h"

namespace rt {
	/**
	 * specifies the image quality of the image resolution
	 */
	enum class Resolution {
		THUMBNAIL,
		LOW,
		MEDIUM,
		HIGH
	};
	/**
	 * specifies the number of rays per pixel
	 */
	enum class Samples {
		ONE,
		LOW,
		MEDIUM,
		HIGH
	};
	/**
	 * specifies the ray tracing depth
	 */
	enum class TraceDepth {
		LOW,
		MEDIUM,
		HIGH
	};

	/**
	 * returns the appropriate tuple of width and height for the given resolution
	 * @param res - resolution specifier
	 * @return pair of width and height
	 */
	inline std::pair<size_t, size_t> determine_resolution(Resolution res) {
		switch (res) {
		case Resolution::THUMBNAIL:
			return std::make_pair(160, 120);
		case Resolution::LOW:
			return std::make_pair(320, 240);
		case Resolution::MEDIUM:
			return std::make_pair(640, 480);
		case Resolution::HIGH:
			return std::make_pair(1280, 960);
		default:
			return std::make_pair(160, 120);
		}
	}
	/**
	 * returns the appropriate number of rays per pixel for the given samples
	 * @param samples - samples specifier
	 * @return number of rays per pixel
	 */
	inline size_t determine_samples(Samples samples) {
		switch (samples) {
		case Samples::ONE:
			return 1;
		case Samples::LOW:
			return 10;
		case Samples::MEDIUM:
			return 100;
		case Samples::HIGH:
			return 1000;
		default:
			return 10;
		}
	}
	/**
	 * returns the appropriate ray tracing depth for the given depth
	 * @param tracedepth - depth specifier
	 * @return ray tracing depth
	 */
	inline size_t determine_trace_depth(TraceDepth tracedepth) {
		switch (tracedepth) {
		case TraceDepth::LOW:
			return 10;
		case TraceDepth::MEDIUM:
			return 50;
		case TraceDepth::HIGH:
			return 100;
		default:
			return 10;
		}
	}

	class ITracer {
	public:
		/**
		 * setter for the scene of hitables
		 * @param scene - hitable or organizing hitable that represents the scene
		 */
		virtual void setHitable(std::shared_ptr<IHitable> scene) = 0;
		/**
		 * setter for the camera to use for tracing
		 * @param camera - camera to trace rays from
		 */
		virtual void setCamera(std::shared_ptr<Camera> cam) = 0;
		/**
		 * setter for the color of the background
		 * @param color - color of the background
		 */
		virtual void setBackgroundColor(vec3 color) = 0;

		/**
		 * returns the aspect ratio with/height of the output image
		 * @return aspect ratio of the output image
		 */
		virtual double aspect() const = 0;

		/**
		 * performs the tracing
		 */
		virtual void run() = 0;
		/**
		 * writes the output image to file
		 * @param filepath of the output png image
		 */
		virtual void write(std::string filepath) const = 0;
	};
}

#endif//I_TRACER_H