#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

size_t rt::Image::width()    const { return m_width;    }
size_t rt::Image::height()   const { return m_height;   }
size_t rt::Image::channels() const { return m_channels; }

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

void rt::Image::write(std::string filename) const {
    stbi_write_png(filename.c_str(), m_width, m_height, m_channels, m_data.data(), 0);
}

bool rt::Image::read(std::string filename) {
	// load data
	int tempwidth, tempheight, tempchannels;
	unsigned char* data = stbi_load(filename.c_str(), &tempwidth, &tempheight, &tempchannels, 3);
	
	// set image dimensions
	m_width = tempwidth;
	m_height = tempheight;
	m_channels = tempchannels;
	
	// adapt channels if number of channels is not 3
	if (tempchannels != 3) {
		console::print("Channels is not 3");
		m_channels = 3;
	}

	// error handling
	if (data == nullptr) return false;

	// convert data to float in the range [0,1]
	m_data.resize(m_width*m_height*m_channels);
	for (size_t y = 0; y < m_height; ++y) {
		for (size_t x = 0; x < m_width; ++x) {
			size_t idx = index(x, y);
			for (size_t c = 0; c < m_channels; ++c) {
				m_data.at(idx + c) = data[idx + c];
			}
		}
	}

	inv_gamma();

	// release data
	stbi_image_free(data);

	return true;
}