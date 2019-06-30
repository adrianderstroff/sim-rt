#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>

#include "console.h"
#include "math/vec3.h"

namespace rt {
class Image {
public:
	Image() {}
    Image(int width, int height, int channels) { 
        m_width    = width; 
        m_height   = height; 
        m_channels = channels; 

        m_data.resize(m_width*m_height*m_channels);
    }

	/**
	 * returns width of the image
	 * @return width of the image
	 */
	int width() const;
	/**
	 * returns height of the image
	 * @return height of the image
	 */
	int height() const;
	/**
	 * returns number of color channels
	 * @return number of color channels
	 */
	int channels() const;

	/**
	 * sets pixel color at position (x,y)
	 * @param x - x-position of the pixel
	 * @param y - y-position of the pixel
	 * @param col - color to update
	 */
    void set(int x, int y, const vec3& col);
	/**
	 * gets pixel color at position (x,y)
	 * @param x - x-position of the pixel
	 * @param y - y-position of the pixel
	 * @return col at (x,y)
	 */
	vec3 get(int x, int y) const;

	/**
	 * writes current image data to file
	 * @param filename - path to png image file
	 */
    void write(std::string filename) const;
	/**
	 * reads in the file and updates image dimensions and data
	 * @param filename - path to a png image file
	 * @return true if loading was successful, false otherwise
	 */
	bool read(std::string filename);

private:
    int m_width;
    int m_height;
    int m_channels;
    std::vector<unsigned char> m_data;

	/**
	 * calculates the 1D index of the pixel position (x,y)
	 * @param x - x-position of the pixel
	 * @param y - y-position of the pixel
	 * @return 1D pixel position
	 */
	size_t index(int x, int y) const;
};
}

#endif//IMAGE_H