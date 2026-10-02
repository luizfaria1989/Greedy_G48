#ifndef MODELO_IMAGE_H
#define MODELO_IMAGE_H

#include <string>

#include "vector_uint8_t_.hpp"

namespace modelo
{
class Image
{
private:
	int width;

	int height;

	vector_uint8_t_ pixels;


public:
	Image(int width, int height);

	int getWidth();

	int getHeight();

	uint8_t getPixel(int x, int y, int c);

	void setPixel(int x, int y, int c, uint8_t value);

};

}  // namespace modelo
#endif
