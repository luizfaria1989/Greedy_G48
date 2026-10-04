#ifndef ALGORITMOS_PREDICAO_UP_PREDICTOR_H
#define ALGORITMOS_PREDICAO_UP_PREDICTOR_H

#include "Predictor.hpp"

namespace algoritmos::predicao {

class UpPredictor : public Predictor {

public:
	std::string getName() const override { return "UpPredictor"; };

protected:
	std::uint8_t predict(const modelo::Image& img, int x, int y, int c) const override {
		return (y > 0) ? img.getPixel(x, y -1, c) : 0;
	};
};

}  // namespace algoritmos::predicao
#endif
