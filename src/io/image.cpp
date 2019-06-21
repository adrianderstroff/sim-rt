#include "image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void Image::set(int x, int y, const vec3& col) {
    int idx = (x+(m_height-y-1)*m_width)*m_channels;
    m_data.at(idx+0) = char(255.99*col.r);
    m_data.at(idx+1) = char(255.99*col.g);
    m_data.at(idx+2) = char(255.00*col.b);
}

void Image::write(std::string filename) {
    stbi_write_png(filename.c_str(), m_width, m_height, m_channels, m_data.data(), 0);
}