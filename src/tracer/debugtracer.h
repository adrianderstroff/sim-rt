#ifndef DEBUGTRACER_H
#define DEBUGTRACER_H

#include <chrono>
#include <string>

#include "hitable/object/cylinder.h"
#include "hitable/object/sphere.h"
#include "hitable/organization/bvh.h"
#include "itracer.h"
#include "io/image.h"
#include "io/console.h"
#include "material/dielectric.h"
#include "material/imaterial.h"
#include "material/lambertian.h"
#include "tracer/raycaster.h"
#include "tracer/raytracer.h"
#include "texture/constanttexture.h"
#include "util/string.h"

namespace rt {
	class Debugtracer : public ITracer {
	public:
		Debugtracer(Resolution r = Resolution::MEDIUM, Samples s = Samples::MEDIUM, TraceDepth t = TraceDepth::MEDIUM);

		void setHitable(std::shared_ptr<IHitable> h) override { m_world = h; }
		void setCamera(std::shared_ptr <Camera> cam) override { m_camera = cam; }
		void setBackgroundColor(vec3 color) override { m_backgroundcolor = color; }

		double aspect() const override { return static_cast<double>(m_width) / static_cast<double>(m_height); }

		void run() override;
		void write(std::string filepath) const override;

	private:
		Raytracer                 m_raycaster;
		std::shared_ptr<Camera>   m_camera, m_rendercamera;
		std::shared_ptr<IHitable> m_world;
		unsigned int              m_width, m_height, m_samples, m_maxdepth;
		vec3                      m_backgroundcolor;

		void build_bounds(std::shared_ptr<BVH>& scene);
		void build_ray_scene(std::shared_ptr<BVH>& scene);
		void trace(const ray& r, std::vector<vec3>& raypoints, int depth) const;

		void setup_render_camera();
		void render_rays(std::shared_ptr<BVH> scene);
	};
}

#endif//DEBUGTRACER_H