#ifndef MODELO_IMAGE_H
#define MODELO_IMAGE_H

#include <cstddef>
#include <cstdint>
#include <vector>

namespace modelo {

class Image {
public:

	Image(int width, int height);

	int getWidth() const { return width; }
	int getHeight() const { return height; }

	std::uint8_t getPixel(int x, int y, int c) const;
	void setPixel(int x, int y, int c, std::uint8_t value);


private:

	std::size_t index(int x, int y, int c) const;

	int width;
	int height;
	std::vector<uint8_t> pixels;


};

}  // namespace modelo
#endif
