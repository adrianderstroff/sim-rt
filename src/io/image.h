#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>

#include "math/vec3.h"

class Image {
public:
	Image() {}
    Image(int width, int height, int channels) { 
        m_width    = width; 
        m_height   = height; 
        m_channels = channels; 

        m_data.resize(m_width*m_height*m_channels);
    }

    void set(int x, int y, const vec3& col);
    void write(std::string filename);

private:
    int m_width;
    int m_height;
    int m_channels;
    std::vector<char> m_data;
};

#endif//IMAGE_H