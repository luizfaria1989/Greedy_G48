#ifndef ALGORITMOS_PREDICAO_PAETH_PREDICTOR_H
#define ALGORITMOS_PREDICAO_PAETH_PREDICTOR_H

#include "algoritmos/predicao/Predictor.hpp"

namespace algoritmos::predicao {

class PaethPredictor : public Predictor {

public:
	std::string getName() const override { return "PaethPredictor"; }

protected:
	uint8_t predict(const modelo::Image& img, int x, int y, int c) const override;

};

}  // namespace algoritmos::predicao
#endif
