#ifndef ALGORITMOS_PREDICAO_LEFT_PREDICTOR_H
#define ALGORITMOS_PREDICAO_LEFT_PREDICTOR_H

#include "Predictor.hpp"

namespace algoritmos::predicao {

	class LeftPredictor : public Predictor {

	public:
		std::string getName() const override { return "LeftPredictor"; };

	protected:
		std::uint8_t predict(const modelo::Image& img, int x, int y, int c) const override {
			return (x > 0) ? img.getPixel(x - 1, y, c) : 0;
		};
	};
}  // namespace algoritmos::predicao
#endif
