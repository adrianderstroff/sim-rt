#ifndef PHOTON_MAPPER_H
#define PHOTON_MAPPER_H

#include "io/image.h"
#include "itracer.h"

namespace rt {
	class PhotonMapper: public ITracer {
	public:
		PhotonMapper(unsigned int width, unsigned int height, unsigned int samples, size_t maxdepth = 50);
		PhotonMapper(Resolution r = Resolution::MEDIUM, Samples s = Samples::MEDIUM, TraceDepth t = TraceDepth::MEDIUM);

		void setHitable(std::shared_ptr<IHitable> h)  override { m_world = h; }
		void setCamera(std::shared_ptr <ICamera> cam) override { m_camera = cam; }
		void setBackgroundColor(vec3 color) override { m_backgroundcolor = color; }

		double aspect() const override { return static_cast<double>(m_width) / static_cast<double>(m_height); }

		void run() override;
		void write(std::string filepath) const override;

	private:
		Image                     m_image;
		std::shared_ptr<ICamera>  m_camera;
		std::shared_ptr<IHitable> m_world;
		unsigned int              m_width, m_height, m_samples, m_maxdepth;
		vec3                      m_backgroundcolor;

		vec3 trace(const ray& r) const;
	};
}

#endif//PHOTON_MAPPER_H