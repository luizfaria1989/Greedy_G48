#include "Predictor.hpp"

namespace algoritmos::predicao {

	modelo::Image Predictor::encode(const modelo::Image& img) const {

		int width = img.getWidth();
		int height = img.getHeight();

		modelo::Image res(width, height);

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				for (int c = 0; c < 3; c++) {
					std::uint8_t pred = predict(img, x, y, c);
					std::uint8_t value = img.getPixel(x, y, c);
					res.setPixel(x, y, c, static_cast<std::uint8_t>(value - pred));
				}
			}
		}

		return res;
	}

	modelo::Image Predictor::decode(const modelo::Image& res) const {

		int width = res.getWidth();
		int height = res.getHeight();

		modelo::Image out(width, height);

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				for (int c = 0; c < 3; c++) {
					std::uint8_t pred = predict(out, x, y, c);
					out.setPixel(x, y, c, static_cast<std::uint8_t>(res.getPixel(x, y, c) + pred));
				}
			}
		}
		return out;
	}

}  // namespace algoritmos::predicao
