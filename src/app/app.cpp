#include "app.h"

App::App(unsigned int width, unsigned int height, unsigned int samples) 
	: m_width(width), m_height(height), m_samples(samples) {
	m_image = Image(width, height, 3);
}

void App::run() {
	// iterate over all pixels
	int size = m_width * m_height;
	for (int i = 0; i < size; i++) {
		// print progress
		console::progress("raytracing", static_cast<double>(i) / static_cast<double>(size - 1));

		// determine x and y index
		unsigned int x = i % m_width;
		unsigned int y = i / m_width;

		// aggregate color for each sample
		vec3 col(0, 0, 0);
		for (int s = 0; s < m_samples; s++) {
			double u = static_cast<double>(x + drand()) / static_cast<double>(m_width);
			double v = static_cast<double>(y + drand()) / static_cast<double>(m_height);
			ray r = m_camera.getRay(u, v);
			col += color(r, 0);
		}
		col /= m_samples;

		// gamma correction
		col = vec3(sqrt(col.r), sqrt(col.g), sqrt(col.b));

		// write to image
		m_image.set(x, y, col);
	}
}

void App::write(std::string pngfilepath) {
	m_image.write(pngfilepath);
}

vec3 App::color(const ray& r, int depth) {
	HitRecord rec;
	if (m_world->hit(r, 0.001, 1'000'000, rec)) {
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.material->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, depth + 1);
		} else {
			return vec3(0, 0, 0);
		}
	}
	else {
		vec3 dir = normalize(r.dir);
		double t = 0.5 * (dir.y + 1.0);
		return lerp(vec3(1.0, 1.0, 1.0), vec3(0.5, 0.7, 1.0), t);
	}
}