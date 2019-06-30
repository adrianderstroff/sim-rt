#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int rt::Image::width() const { return m_width; }
int rt::Image::height() const { return m_height; }
int rt::Image::channels() const { return m_channels; }

size_t rt::Image::index(int x, int y) const {
	return (x + (m_height - y - 1)*m_width)*m_channels;
}

void rt::Image::set(int x, int y, const vec3& col) {
    int idx = index(x, y);
    m_data.at(idx+0) = char(255.99*col.r);
    m_data.at(idx+1) = char(255.99*col.g);
    m_data.at(idx+2) = char(255.99*col.b);
}

rt::vec3 rt::Image::get(int x, int y) const {
	size_t idx = index(x, y);
	float r = static_cast<float>(m_data.at(idx + 0)) / 255.f;
	float g = static_cast<float>(m_data.at(idx + 1)) / 255.f;
	float b = static_cast<float>(m_data.at(idx + 2)) / 255.f;

	return vec3(r, g, b);
}

void rt::Image::write(std::string filename) const {
    stbi_write_png(filename.c_str(), m_width, m_height, m_channels, m_data.data(), 0);
}

bool rt::Image::read(std::string filename) {
	// load data
	unsigned char* data = stbi_load(filename.c_str(), &m_width, &m_height, &m_channels, 3);
	if (m_channels != 3) console::print("Channels is not 3");

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

	// release data
	stbi_image_free(data);

	return true;
}