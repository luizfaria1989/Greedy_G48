#ifndef ALGORITMOS_PREDICAO_PAETH_PREDICTOR_H
#define ALGORITMOS_PREDICAO_PAETH_PREDICTOR_H

#include <string>
#include <vector>
#include <list>
#include <assert.h>

#include "algoritmos/predicao/Predictor.hpp"
#include "modelo/Image.hpp"

namespace algoritmos
{
namespace predicao
{
class PaethPredictor : public Predictor
{
protected:
	uint8_t predict(modelo::Image img, int x, int y, int c);

public:
	std::string getName();

};

}  // namespace predicao
}  // namespace algoritmos
#endif
