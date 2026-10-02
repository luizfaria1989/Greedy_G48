#ifndef ALGORITMOS_PREDICAO_PREDICTOR_H
#define ALGORITMOS_PREDICAO_PREDICTOR_H

#include <string>

#include "modelo/Image.hpp"

namespace algoritmos
{
namespace predicao
{
class Predictor
{
protected:
	virtual uint8_t predict(modelo::Image img, int x, int y, int c)=0;

public:
	modelo::Image encode(modelo::Image img);

	modelo::Image decode(modelo::Image res);

	virtual std::string getName()=0;

	˜Predictor();

};

}  // namespace predicao
}  // namespace algoritmos
#endif
