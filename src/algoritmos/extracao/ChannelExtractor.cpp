#include <string>
#include <vector>
#include <list>

#include "ChannelExtractor.hpp"

namespace algoritmos::extracao {

	std::vector<SymbolStream> ChannelExtractor::extract(const modelo::Image& img) const {

		int width = img.getWidth();
		int height = img.getHeight();

		std::vector<SymbolStream> streams(3);

		std::size_t total = static_cast<std::size_t>(width) * height;

		for (int c = 0; c < 3; c++) {
			streams[c].reserve(total);
		}

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				for (int c = 0; c < 3; c++) {
					streams[c].push_back(img.getPixel(x, y, c));
				}
			}
		}
		return streams;
	}

	// modelo::Image ChannelExtractor::rebuild(const std::vector<SymbolStream>& streams, int width, int height) const {
	//
	// 	if (streams.size() != 3) {
	// 		throw std::invalid_argument("Invalid number of streams");
	// 	}
	//
	// 	modelo::Image img(width, height);
	//
	// 	for (int y = 0; y < height; y++) {
	// 		for (int x = 0; x < width; x++) {
	// 			for (int c = 0; c < 3; c++) {
	// 				if (streams[c][x, y] != 255) {
	// 					throw std::invalid_argument("arquivo corrompido");
	// 				}
	// 				img.setPixel(x, y, c, static_cast<std::uint8_t>(valor));
	// 			}
	// 		}
	// 	}
	// 	return img;
	// }

	std::string ChannelExtractor::getName() const {
		return "ChannelExtractor";
	}

}  // namespace algoritmos::extracao
