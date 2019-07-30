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
	enum DebugMode {
		LINES,
		POINTS
	};

	class Debugtracer : public ITracer {
	public:
		Debugtracer(unsigned int width, unsigned int height, unsigned int samples, size_t maxdepth = 50);
		Debugtracer(Resolution r = Resolution::MEDIUM, Samples s = Samples::MEDIUM, TraceDepth t = TraceDepth::MEDIUM);

		// overwritten getters and setters
		void setHitable(std::shared_ptr<IHitable> h)  override { m_world = h; }
		void setCamera(std::shared_ptr <ICamera> cam) override { m_camera = cam; }
		void setBackgroundColor(vec3 color) override { m_backgroundcolor = color; }
		double aspect() const override { return static_cast<double>(m_width) / static_cast<double>(m_height); }

		// debug tracer specific functions
		void setDebugMode(DebugMode debugmode) { m_debugmode = debugmode; }
		void setLineWidth(double width) { m_linewidth = width; }
		void setPointSize(double size) { m_pointsize = size; }
		void setRendererResolution(size_t width, size_t height) {
			m_renderer_width = width;
			m_renderer_height = height;
			m_raycaster = Raycaster(m_renderer_width, m_renderer_height, m_renderer_samples, 1);
		}
		void setRendererSamples(size_t samples) { 
			m_renderer_samples = samples;
			m_raycaster = Raycaster(m_renderer_width, m_renderer_height, m_renderer_samples, 1);
		}

		// overwritten functions
		void run() override;
		void write(std::string filepath) const override;

	private:
		Raycaster                 m_raycaster;
		std::shared_ptr<ICamera>  m_camera;
		std::shared_ptr<IHitable> m_world;
		size_t                    m_width, m_height, m_samples, m_maxdepth;
		vec3                      m_backgroundcolor;
		DebugMode                 m_debugmode;
		// rendering related members
		std::shared_ptr<ICamera> m_rendercamera;
		size_t                   m_renderer_width, m_renderer_height, m_renderer_samples;
		double m_linewidth;
		double m_pointsize;


		void build_camera(std::shared_ptr<BVH>& scene);
		void build_bounds(std::shared_ptr<BVH>& scene);
		void build_ray_scene(std::shared_ptr<BVH>& scene);
		void trace(const ray& r, std::vector<vec3>& raypoints, int depth) const;

		void setup_render_camera();
		void render_rays(std::shared_ptr<BVH> scene);
	};
}

#endif//DEBUGTRACER_H