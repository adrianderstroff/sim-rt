#ifndef APP_H
#define APP_H

#include <string>

#include "../hitable/hitable.h"
#include "../scene/camera.h"
#include "../io/image.h"
#include "../io/console.h"
#include "../material/material.h"

class App {
public:
	App(unsigned int width, unsigned int height, unsigned int samples);

	void setHitable(std::shared_ptr<Hitable> h) { m_world = h; }
	void setCamera(Camera& cam) { m_camera = cam; }
	
	double aspect() { return static_cast<double>(m_width) / static_cast<double>(m_height); }

	void run();
	void write(std::string pngfilepath);

private:
	Image                    m_image;
	Camera                   m_camera;
	std::shared_ptr<Hitable> m_world;
	unsigned int             m_width, m_height, m_samples;

	vec3 color(const ray& r, int depth);
};

#endif//APP_H