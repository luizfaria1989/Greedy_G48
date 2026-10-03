#include <string>
#include <vector>
#include <list>

#include "PackedPixelExtractor.hpp"

namespace algoritmos::extracao {

	// std::vector<SymbolStream> PackedPixelExtractor::extract(const modelo::Image& img) const {
	//
	// 	SymbolStream stream;
	//
	// 	int width = img.getWidth();
	// 	int height = img.getHeight();
	//
	// 	for (int y = 0; y < height; y++) {
	// 		for (int x = 0; x < width; x++) {
	// 			std::uint32_t r = static_cast<uint32_t>(img.getPixel(x, y, 0));
	// 			std::uint32_t g = static_cast<uint32_t>(img.getPixel(x, y, 1));
	// 			std::uint32_t b = static_cast<uint32_t>(img.getPixel(x, y, 2));
	//
	// 			std::uint32_t symbol = (r << 16) | (g << 8) | b;
	// 			stream.push_back(symbol);
	// 		}
	// 	}
	//
	// 	return {std::move(stream)};
	// }

	// modelo::Image PackedPixelExtractor::rebuild(std::vector<SymbolStream>& streams, int width, int height) const {
	//
	// 	if (streams.size() != width * height) {
	// 		throw std::invalid_argument("Image com menos simbolos");
	// 	}
	//
	// 	modelo::Image img(width, height);
	//
	// 	for (int y = 0; y < height; y++) {
	// 		for (int x = 0; x < width; x++) {
	//
	// 		}
	// 	}
	//
	// 	return 0;
	// }

	std::string PackedPixelExtractor::getName() const {
		return "PackedPixelExtractor";
	}

}  // namespace algoritmos::extracao
