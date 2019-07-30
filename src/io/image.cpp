#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

size_t rt::Image::width()    const { return m_width;    }
size_t rt::Image::height()   const { return m_height;   }
size_t rt::Image::channels() const { return m_channels; }
const std::vector<unsigned char>& rt::Image::data() const { return m_data; }

size_t rt::Image::index(size_t x, size_t y) const {
	return (x + y*m_width)*m_channels;
}

void rt::Image::set(size_t x, size_t y, const vec3& col) {
	size_t idx = index(x, y);
    m_data.at(idx+0) = static_cast<unsigned char>(255.99*col.r);
    m_data.at(idx+1) = static_cast<unsigned char>(255.99*col.g);
    m_data.at(idx+2) = static_cast<unsigned char>(255.99*col.b);
}

rt::vec3 rt::Image::get(size_t x, size_t y) const {
	size_t idx = index(x, y);
	float r = static_cast<float>(m_data.at(idx + 0)) / 255.f;
	float g = static_cast<float>(m_data.at(idx + 1)) / 255.f;
	float b = static_cast<float>(m_data.at(idx + 2)) / 255.f;

	return vec3(r, g, b);
}

void rt::Image::clear(const vec3& col) {
	unsigned char r = static_cast<unsigned char>(255.99 * col.r);
	unsigned char g = static_cast<unsigned char>(255.99 * col.g);
	unsigned char b = static_cast<unsigned char>(255.99 * col.b);

	size_t size = m_width * m_height * m_channels;
	for (size_t i = 2; i < size; i+=3) {
		m_data[i - 2] = r;
		m_data[i - 1] = g;
		m_data[i - 0] = b;
	}
}

void rt::Image::inv_gamma() {
	for (size_t y = 0; y < m_height; ++y) {
		for (size_t x = 0; x < m_width; ++x) {
			vec3 pixel = get(x, y);
			pixel.x *= pixel.x;
			pixel.y *= pixel.y;
			pixel.z *= pixel.z;
			set(x, y, pixel);
		}
	}
}

void rt::write_image(std::string filename, const Image& image) {
    stbi_write_png(filename.c_str(), image.width(), image.height(), image.channels(), image.data().data(), 0);
}

std::pair<rt::Image, bool> rt::read_image(std::string filename) {
	// load data
	int tempwidth, tempheight, tempchannels;
	unsigned char* data = stbi_load(filename.c_str(), &tempwidth, &tempheight, &tempchannels, 3);
	
	// adapt channels if number of channels is not 3
	if (tempchannels != 3) {
		console::print("Channels is not 3");
		tempchannels = 3;
	}

	// error handling
	if (data == nullptr) return std::make_pair(Image(), false);

	// create image
	Image image(tempwidth, tempheight, tempchannels);

	// convert data to float in the range [0,1]
	for (size_t y = 0; y < tempheight; ++y) {
		for (size_t x = 0; x < tempwidth; ++x) {
			// determine index
			size_t idx = (x + y * tempwidth) * tempchannels;
			// normalize values
			double r = data[idx + 0] / 255.0;
			double g = data[idx + 1] / 255.0;
			double b = data[idx + 2] / 255.0;
			vec3 col(r*r, g*g, b*b);
			// inverse gamma
			image.set(x, y, col);
		}
	}

	// release data
	stbi_image_free(data);

	return std::make_pair(image, true);
}