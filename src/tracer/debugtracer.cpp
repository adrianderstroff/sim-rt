#include "debugtracer.h"

rt::Debugtracer::Debugtracer(unsigned int width, unsigned int height, unsigned int samples, size_t maxdepth)
	: m_backgroundcolor(0, 0, 0), m_width(width), m_height(height), m_samples(samples), m_maxdepth(maxdepth), m_debugmode(DebugMode::POINTS),
	  m_linewidth(0.001), m_pointsize(0.01), m_renderer_width(width), m_renderer_height(height), m_renderer_samples(samples) {
	m_raycaster = Raycaster(width, height, samples, 1);
}

rt::Debugtracer::Debugtracer(Resolution r, Samples s, TraceDepth t) 
	: m_backgroundcolor(0, 0, 0), m_debugmode(DebugMode::POINTS), m_linewidth(0.001), m_pointsize(0.01) {
	// determine resolution
	auto [width, height] = determine_resolution(r);
	m_width = width; m_height = height;
	m_renderer_width = width;
	m_renderer_height = height;

	// determine number of samples
	m_samples = determine_samples(s);
	m_renderer_samples = m_samples;

	// determine ray tracing depth
	m_maxdepth = determine_trace_depth(t);

	// setup raycaster
	m_raycaster = Raycaster(r, s, TraceDepth::ONE);
}

void rt::Debugtracer::run() {
	// print tracer settings
	console::println("TYPE  : Debugtracer");
	console::println("RES   : " + std::to_string(m_width) + "x" + std::to_string(m_height));
	console::println("MSAA  : " + std::to_string(m_samples));
	console::println("DEPTH : " + std::to_string(m_maxdepth));

	// initialize scene 
	std::shared_ptr<BVH> scene = std::make_shared<BVH>(2, 20);
	build_camera(scene);
	build_bounds(scene);
	build_ray_scene(scene);

	// setup render camera
	setup_render_camera();

	// render scene from new angle
	render_rays(scene);
}

void rt::Debugtracer::write(std::string filepath) const {
	m_raycaster.write(filepath);
}

void rt::Debugtracer::build_camera(std::shared_ptr<BVH>& scene) {
	auto cameramaterial = std::make_shared<Lambertian>(new_color(vec3(0, 0, 1)));

	// camera is a sphere
	scene->insert(std::make_shared<Sphere>(m_camera->get_position(), 5*m_pointsize, cameramaterial));

	// camera image frame
	vec3 origin = m_camera->get_image_plane_origin();
	vec3 x = m_camera->get_image_plane_xaxis();
	vec3 y = m_camera->get_image_plane_yaxis();


	scene->insert(std::make_shared<Cylinder>(origin, origin + x, m_linewidth, cameramaterial));
	scene->insert(std::make_shared<Cylinder>(origin + x, origin + x + y, m_linewidth, cameramaterial));
	scene->insert(std::make_shared<Cylinder>(origin + x + y, origin + y, m_linewidth, cameramaterial));
	scene->insert(std::make_shared<Cylinder>(origin + y, origin, m_linewidth, cameramaterial));
}

void rt::Debugtracer::build_bounds(std::shared_ptr<BVH>& scene) {
	aabb bounds;
	if (m_world->boundingbox(bounds)) {
		auto linematerial = std::make_shared<Lambertian>(new_color(vec3(0)));

		bounds.extend(m_camera->get_position());
		vec3 dir = bounds.max() - bounds.min();
		vec3 origin = bounds.min();
		vec3 x = vec3(dir.x, 0, 0);
		vec3 y = vec3(0, dir.y, 0);
		vec3 z = vec3(0, 0, dir.z);

		float linewidth = 0.001;

		scene->insert(std::make_shared<Cylinder>(origin, origin+x, linewidth, linematerial));
		scene->insert(std::make_shared<Cylinder>(origin + x, origin + x + y, linewidth, linematerial));
		scene->insert(std::make_shared<Cylinder>(origin + x + y, origin + y, linewidth, linematerial));
		scene->insert(std::make_shared<Cylinder>(origin + y, origin, linewidth, linematerial));

		scene->insert(std::make_shared<Cylinder>(origin + z, origin + z + x, linewidth, linematerial));
		scene->insert(std::make_shared<Cylinder>(origin + z + x, origin + z + x + y, linewidth, linematerial));
		scene->insert(std::make_shared<Cylinder>(origin + z + x + y, origin + z + y, linewidth, linematerial));
		scene->insert(std::make_shared<Cylinder>(origin + z + y, origin + z, linewidth, linematerial));

		scene->insert(std::make_shared<Cylinder>(origin, origin + z, linewidth, linematerial));
		scene->insert(std::make_shared<Cylinder>(origin + x, origin + x + z, linewidth, linematerial));
		scene->insert(std::make_shared<Cylinder>(origin + x + y, origin + x + y + z, linewidth, linematerial));
		scene->insert(std::make_shared<Cylinder>(origin + y, origin + y + z, linewidth, linematerial));
	}
}

void rt::Debugtracer::build_ray_scene(std::shared_ptr<BVH>& scene) {
	// start timer
	auto starttime = std::chrono::high_resolution_clock::now();

	// setup red material
	auto redmaterial = std::make_shared<Lambertian>(new_color(vec3(1, 0, 0)));

	// calculate step
	size_t stepx = m_width / 50;
	size_t stepy = m_height / 50;

	// iterate over all pixels
	size_t size = m_width * m_height;
	for (size_t y = 0; y < m_height; y += stepy) {
		for (size_t x = 0; x < m_width; x += stepx) {
			// print progress
			size_t i = x + y*m_width;
			console::progress("build scene", static_cast<double>(i) / static_cast<double>(size - 1));

			// trace ray and store all of it's intersection points
			for (size_t s = 0; s < m_samples; ++s) {
				double u = static_cast<double>(x + drand()) / static_cast<double>(m_width);
				double v = static_cast<double>(y + drand()) / static_cast<double>(m_height);
				ray r = m_camera->get_ray(u, v);

				// trace ray and store its points
				std::vector<vec3> raypoints;
				raypoints.push_back(r.o);
				trace(r, raypoints, 0);

				// create debug primitives that will be visualized in the next step
				for (size_t s = 1; s < raypoints.size(); ++s) {
					switch (m_debugmode) {
					case DebugMode::LINES:
						// create a cylinder for each track of the ray
						scene->insert(std::make_shared<Cylinder>(raypoints.at(s - 1), raypoints.at(s), m_linewidth, redmaterial));
						break;
					case DebugMode::POINTS:
						// create spheres for each ray intersection
						scene->insert(std::make_shared<Sphere>(raypoints.at(s), m_pointsize, redmaterial));
						break;
					}
					
				}
			}
		}
	}

	// build scene
	scene->build();

	// print elapsed time
	auto endtime = std::chrono::high_resolution_clock::now();
	auto elapsedtime = std::chrono::duration_cast<std::chrono::duration<double>>(endtime - starttime);
	console::println("elapsed time: " + format_time(elapsedtime.count()));
}

void rt::Debugtracer::trace(const ray& r, std::vector<vec3>& raypoints, int depth) const {
	HitRecord rec;
	if (m_world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.material->emitted(rec.u, rec.v, rec.lp);
		if (depth < m_maxdepth && rec.material->scatter(r, rec, attenuation, scattered)) {
			raypoints.push_back(rec.p);
			return trace(scattered, raypoints, depth + 1);
		}
	}
	
	// just extend point
	raypoints.push_back(r.position(10.0));
}

void rt::Debugtracer::setup_render_camera() {
	aabb bounds;
	if (m_world->boundingbox(bounds)) {
		// get the direction of the camera
		vec3 view = normalize(m_camera->get_ray(0.5, 0.5).dir);
		vec3 dir = cross(view, vec3(0, 1, 0));

		// get bounding box of the whole scene
		bounds.extend(m_camera->get_position());
		vec3 center = bounds.center();
		double extent = length(bounds.max() - bounds.min());

		// set actual camera
		vec3 pos = center + extent * dir;
		vec3 lookat = center;
		m_rendercamera = std::make_shared<SimpleCamera>(pos, lookat, vec3(0, 1, 0), 45.0, aspect());
	}
	else {
		m_rendercamera = m_camera;
	}
}

void rt::Debugtracer::render_rays(std::shared_ptr<BVH> scene) {
	m_raycaster.setBackgroundColor(m_backgroundcolor);
	m_raycaster.setHitable(scene);
	m_raycaster.setCamera(m_rendercamera);
	m_raycaster.run();
}
