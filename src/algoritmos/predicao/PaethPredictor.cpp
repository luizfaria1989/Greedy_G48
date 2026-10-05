#include "PaethPredictor.hpp"

#include <cstdlib>

namespace algoritmos::predicao {

std::uint8_t PaethPredictor::predict(const modelo::Image& img, int x, int y, int c) const {

	int left = (x > 0) ? img.getPixel(x - 1, y, c) : 0;
	int up = (y > 0 ) ? img.getPixel(x, y - 1, c) : 0;
	int upLeft = (x > 0 && y > 0) ? img.getPixel(x - 1, y - 1, c) : 0;

	int p = left + up - upLeft;

	int pa = std::abs(p - left);
	int pb = std::abs(p - up);
	int pc = std::abs(p - upLeft);

	if (pa <= pb && pa <= pc) {
		return static_cast<std::uint8_t>(left);
	} else if (pb <= pc) {
		return static_cast<std::uint8_t>(up);
	}

	return static_cast<std::uint8_t>(upLeft);
}

}  // namespace algoritmos::predicao
