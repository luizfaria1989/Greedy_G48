#ifndef ALGORITMOS_PREDICAO_NO_PREDICTOR_H
#define ALGORITMOS_PREDICAO_NO_PREDICTOR_H

#include "Predictor.hpp"

namespace algoritmos::predicao {

	class NoPredictor : public Predictor {

	public:
		std::string getName() const override { return "NoPredictor"; };

	protected:
		std::uint8_t predict(const modelo::Image& img, int, int, int) const override {
			return 0;
		};

	};

}  // namespace algoritmos::predicao
#endif

