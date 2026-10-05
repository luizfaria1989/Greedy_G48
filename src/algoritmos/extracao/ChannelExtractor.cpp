#include "ChannelExtractor.hpp"

#include <stdexcept>
#include <utility>

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

	modelo::Image ChannelExtractor::rebuild(const std::vector<SymbolStream>& streams, int width, int height) const {

		std::size_t total = static_cast<std::size_t>(width) * height;

		if (streams.size() != 3) {
			throw std::invalid_argument("ChannelExtractor: Invalid number of streams");
		}

		for (int c = 0; c < 3; c++) {
			if (streams[c].size() != total) {
				throw std::invalid_argument("ChannelExtractor: Invalid number of symbols");
			}
		}

		modelo::Image img(width, height);

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				// Mesma posição nos três fluxos: um valor por pixel em cada um.
				std::size_t i = static_cast<std::size_t>(y) * width + x;

				for (int c = 0; c < 3; c++) {
					std::uint32_t valor = streams[c][i];
					if (valor > 255) {
						throw std::invalid_argument("ChannelExtractor: Corrupted file");
					}
					img.setPixel(x, y, c, static_cast<std::uint8_t>(valor));
				}
			}
		}
		return img;
	}

	std::string ChannelExtractor::getName() const {
		return "ChannelExtractor";
	}

}  // namespace algoritmos::extracao
