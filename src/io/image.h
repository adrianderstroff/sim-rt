#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>

#include "console.h"
#include "math/vec3.h"

namespace rt {
	/**
	 * datastructure for reading, modifying and writing images
	 * the first pixel is in the upper left corner
	 */
	class Image {
	public:
		Image() : m_width(0), m_height(0), m_channels(0) {}
		Image(size_t width, size_t height, size_t channels) {
			m_width    = width; 
			m_height   = height; 
			m_channels = channels; 

			m_data.resize(m_width*m_height*m_channels);
		}

		/**
		 * returns width of the image
		 * @return width of the image
		 */
		size_t width() const;
		/**
		 * returns height of the image
		 * @return height of the image
		 */
		size_t height() const;
		/**
		 * returns number of color channels
		 * @return number of color channels
		 */
		size_t channels() const;

		/**
		 * sets pixel color at position (x,y)
		 * @param x - x-position of the pixel
		 * @param y - y-position of the pixel
		 * @param col - color to update
		 */
		void set(size_t x, size_t y, const vec3& col);
		/**
		 * gets pixel color at position (x,y)
		 * @param x - x-position of the pixel
		 * @param y - y-position of the pixel
		 * @return col at (x,y)
		 */
		vec3 get(size_t x, size_t y) const;

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
		size_t m_width;
		size_t m_height;
		size_t m_channels;
		std::vector<unsigned char> m_data;

		/**
		 * calculates the 1D index of the pixel position (x,y)
		 * @param x - x-position of the pixel
		 * @param y - y-position of the pixel
		 * @return 1D pixel position
		 */
		size_t index(size_t x, size_t y) const;

		/**
		 * performs inverse gamma to make sure
		 * that the image contains it's original
		 * color
		 */
		void inv_gamma();
	};
}

#endif//IMAGE_H