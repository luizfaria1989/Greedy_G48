#include "PackedPixelExtractor.hpp"

#include <stdexcept>
#include <utility>

namespace algoritmos::extracao {

	std::vector<SymbolStream> PackedPixelExtractor::extract(const modelo::Image& img) const {

		int width = img.getWidth();
		int height = img.getHeight();

		SymbolStream stream;

		std::size_t total = static_cast<std::size_t>(width) * height;
		stream.reserve(total);

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				std::uint32_t r = static_cast<std::uint32_t>(img.getPixel(x, y, 0));
				std::uint32_t g = static_cast<std::uint32_t>(img.getPixel(x, y, 1));
				std::uint32_t b = static_cast<std::uint32_t>(img.getPixel(x, y, 2));

				std::uint32_t symbol = (r << 16) | (g << 8) | b;
				stream.push_back(symbol);
			}
		}

		return {std::move(stream)};
	}

	modelo::Image PackedPixelExtractor::rebuild(const std::vector<SymbolStream>& streams, int width, int height) const {

		std::size_t total = static_cast<std::size_t>(width) * height;

		if (streams.size() != 1) {
			throw std::invalid_argument("PackedPixelExtractor: esperado exatamente 1 fluxo");
		}

		if (streams[0].size() != total) {
			throw std::invalid_argument("PackedPixelExtractor: quantidade de simbolos incorreta");
		}

		const SymbolStream& stream = streams[0];

		modelo::Image img(width, height);

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				std::size_t i = static_cast<std::size_t>(y) * width + x;
				std::uint32_t symbol = stream[i];
				if (symbol > 0xFFFFFF) {
					throw std::invalid_argument("PackedPixelExtractor: Currupted File");
				}
				img.setPixel(x, y, 0, static_cast<std::uint8_t>((symbol >> 16) & 0xFF));
				img.setPixel(x, y, 1, static_cast<std::uint8_t>((symbol >> 8) & 0xFF));
				img.setPixel(x, y, 2, static_cast<std::uint8_t>(symbol & 0xFF));
			}
		}

		return img;
	}

	std::string PackedPixelExtractor::getName() const {
		return "PackedPixelExtractor";
	}

}  // namespace algoritmos::extracao
