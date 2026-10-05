#ifndef ALGORITMOS_PREDICAO_PREDICTOR_H
#define ALGORITMOS_PREDICAO_PREDICTOR_H

#include "modelo/Image.hpp"

#include <cstdint>
#include <string>

namespace algoritmos::predicao {

class Predictor {

public:
	virtual ~Predictor() =  default;

	modelo::Image encode(const modelo::Image& img) const;
	modelo::Image decode(const modelo::Image& res) const;
	virtual std::string getName() const = 0;

protected:
	virtual std::uint8_t predict(const modelo::Image& img, int x, int y, int c) const = 0;

};

}  // namespace algoritmos::predicao
#endif
