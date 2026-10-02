#ifndef ALGORITMOS_PREDICAO_LEFT_PREDICTOR_H
#define ALGORITMOS_PREDICAO_LEFT_PREDICTOR_H

#include <string>

#include "algoritmos/predicao/Predictor.hpp"
#include "modelo/Image.hpp"

namespace algoritmos
{
namespace predicao
{
class LeftPredictor : public Predictor
{
protected:
	uint8_t predict(modelo::Image img, int x, int y, int c);

public:
	std::string getName();

};

}  // namespace predicao
}  // namespace algoritmos

#endif
