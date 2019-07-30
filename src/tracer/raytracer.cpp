#include "raytracer.h"

rt::Raytracer::Raytracer(size_t width, size_t height, size_t samples, size_t maxdepth)
	: m_width(width), m_height(height), m_samples(samples), m_maxdepth(maxdepth), m_backgroundcolor(0, 0, 0) {
	m_image = Image(width, height, 3);
}

rt::Raytracer::Raytracer(Resolution r, Samples s, TraceDepth t) : m_backgroundcolor(0,0,0) {
	// determine resolution
	auto [width, height] = determine_resolution(r);
	m_width = width; m_height = height;
	m_image = Image(m_width, m_height, 3);

	// determine number of samples
	m_samples = determine_samples(s);

	// determine ray tracing depth
	m_maxdepth = determine_trace_depth(t);
}

void rt::Raytracer::run() {
	// print tracer settings
	console::println("TYPE  : Raytracer");
	console::println("RES   : " + std::to_string(m_width) + "x" + std::to_string(m_height));
	console::println("MSAA  : " + std::to_string(m_samples));
	console::println("DEPTH : " + std::to_string(m_maxdepth));

	// start timer
	auto starttime = std::chrono::high_resolution_clock::now();

	// iterate over all pixels
	size_t size = m_width * m_height;
	for (size_t i = 0; i < size; ++i) {
		// print progress
		console::progress("raytracing", static_cast<double>(i) / static_cast<double>(size - 1));

		// determine x and y index
		size_t x = i % m_width;
		size_t y = i / m_width;

		// aggregate color for each sample
		vec3 col(0, 0, 0);
		for (size_t s = 0; s < m_samples; ++s) {
			double u = static_cast<double>(x + drand()) / static_cast<double>(m_width);
			double v = static_cast<double>(y + drand()) / static_cast<double>(m_height);
			ray r = m_camera->get_ray(u, v);
			col += trace(r, 0);
		}
		col /= m_samples;

		// gamma correction
		col = sqrt(col);

		// set pixel color
		m_image.set(x, y, col);
	}

	// print elapsed time
	auto endtime = std::chrono::high_resolution_clock::now();
	auto elapsedtime = std::chrono::duration_cast<std::chrono::duration<double>>(endtime - starttime);
	console::println("elapsed time: " + format_time(elapsedtime.count()));
}

void rt::Raytracer::write(std::string filepath) const {
	write_image(filepath, m_image);
}

rt::vec3 rt::Raytracer::trace(const ray& r, int depth) const {
	HitRecord rec;
	if (m_world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.material->emitted(rec.u, rec.v, rec.lp);
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