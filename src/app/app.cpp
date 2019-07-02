#include "app.h"

rt::App::App(unsigned int width, unsigned int height, unsigned int samples, size_t maxdepth)
	: m_width(width), m_height(height), m_samples(samples), m_maxdepth(maxdepth), m_backgroundcolor(0, 0, 0) {
	m_image = Image(width, height, 3);
}

void rt::App::run() {
	// start timer
	auto starttime = std::chrono::high_resolution_clock::now();

	// iterate over all pixels
	int size = m_width * m_height;
	for (size_t i = 0; i < size; ++i) {
		// print progress
		console::progress("raytracing", static_cast<double>(i) / static_cast<double>(size - 1));

		// determine x and y index
		unsigned int x = i % m_width;
		unsigned int y = i / m_width;

		// aggregate color for each sample
		vec3 col(0, 0, 0);
		for (size_t s = 0; s < m_samples; ++s) {
			double u = static_cast<double>(x + drand()) / static_cast<double>(m_width);
			double v = static_cast<double>(y + drand()) / static_cast<double>(m_height);
			ray r = m_camera->getRay(u, v);
			col += trace(r, 0);
		}
		col /= m_samples;

		// gamma correction
		col = vec3(sqrt(col.r), sqrt(col.g), sqrt(col.b));

		// set pixel color
		m_image.set(x, y, col);
	}

	// print elapsed time
	auto endtime = std::chrono::high_resolution_clock::now();
	auto elapsedtime = std::chrono::duration_cast<std::chrono::duration<double>>(endtime - starttime);
	console::println("elapsed time: " + std::to_string(elapsedtime.count()) + "s");
}

void rt::App::write(std::string pngfilepath) const {
	m_image.write(pngfilepath);
}

rt::vec3 rt::App::trace(const ray& r, int depth) const {
	HitRecord rec;
	if (m_world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.material->emitted(rec.u, rec.v);
		if (depth < m_maxdepth && rec.material->scatter(r, rec, attenuation, scattered)) {
			return emitted + attenuation * trace(scattered, depth + 1);
		} else {
			return emitted;
		}
	}
	else {
		return m_backgroundcolor;
	}
}