#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <chrono>
#include <string>

#include "hitable/ihitable.h"
#include "io/image.h"
#include "io/console.h"
#include "material/imaterial.h"
#include "scene/camera.h"
#include "util/string.h"

namespace rt {
	enum class Resolution {
		THUMBNAIL,
		LOW,
		MEDIUM,
		HIGH
	};
	enum class Samples {
		LOW,
		MEDIUM,
		HIGH
	};
	enum class TraceDepth {
		LOW,
		MEDIUM,
		HIGH
	};

	class Raytracer {
	public:
		Raytracer(unsigned int width, unsigned int height, unsigned int samples, size_t maxdepth = 50);
		Raytracer(Resolution r = Resolution::MEDIUM, Samples s = Samples::MEDIUM, TraceDepth t = TraceDepth::MEDIUM);

		void setHitable(std::shared_ptr<IHitable> h) { m_world = h; }
		void setCamera(std::shared_ptr <Camera> cam) { m_camera = cam; }
		void setBackgroundColor(vec3 color) { m_backgroundcolor = color; }
	
		double aspect() const { return static_cast<double>(m_width) / static_cast<double>(m_height); }

		void run();
		void write(std::string pngfilepath) const;

	private:
		Image                     m_image;
		std::shared_ptr<Camera>   m_camera;
		std::shared_ptr<IHitable> m_world;
		unsigned int              m_width, m_height, m_samples, m_maxdepth;
		vec3                      m_backgroundcolor;

		vec3 trace(const ray& r, int depth) const;
	};
}

#endif//RAYTRACER_H