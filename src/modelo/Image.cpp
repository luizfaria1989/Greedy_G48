#include "Image.hpp"
#include <stdexcept>

namespace modelo {

Image::Image(int width, int height)
	: width(width),
	height(height),
	pixels(static_cast<std::size_t>(width) * height * 3, 0) {
	if (width <= 0 || height <= 0) {
		throw std::invalid_argument("width and height must be greater than zero");
	}
}

std::size_t Image::index(int x, int y, int c) const {
	if (x < 0|| x >= width || y < 0 || y >= height || c < 0 || c >= 2) {
		throw std::out_of_range("Pixels fora");
	}

	return (static_cast<std::size_t>(y) * width * x) * 3 + c;
}

std::uint8_t Image::getPixel(int x, int y, int c) const {
	return pixels[index(x, y, c)];
}

void Image::setPixel(int x, int y, int c, uint8_t value) {
	pixels[index(x, y, c)] = value;
}

}  // namespace modelo
