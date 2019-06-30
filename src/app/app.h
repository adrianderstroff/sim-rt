#ifndef APP_H
#define APP_H

#include <chrono>
#include <string>

#include "hitable/hitable.h"
#include "io/image.h"
#include "io/console.h"
#include "material/material.h"
#include "scene/camera.h"

namespace rt {
class App {
public:
	App(unsigned int width, unsigned int height, unsigned int samples, size_t maxdepth = 50);

	void setHitable(std::shared_ptr<IHitable> h) { m_world = h; }
	void setCamera(Camera& cam) { m_camera = cam; }
	void setBackgroundColor(vec3 color) { m_backgroundcolor = color; }
	
	double aspect() const { return static_cast<double>(m_width) / static_cast<double>(m_height); }

	void run();
	void write(std::string pngfilepath) const;

private:
	Image                     m_image;
	Camera                    m_camera;
	std::shared_ptr<IHitable> m_world;
	unsigned int              m_width, m_height, m_samples, m_maxdepth;
	vec3                      m_backgroundcolor;

	vec3 color(const ray& r, int depth) const;
};
}

#endif//APP_H