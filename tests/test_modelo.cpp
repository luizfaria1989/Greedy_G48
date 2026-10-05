#include "TestFramework.hpp"
#include "modelo/Image.hpp"

namespace {

// T01: Image(3, 2) tem largura 3 e altura 2.
void dimensoes() {
    modelo::Image img(3, 2);

    CHECK(img.getWidth() == 3);
    CHECK(img.getHeight() == 2);
}

// T02: o valor gravado volta, e o vizinho continua em 0.
void gravarELer() {
    modelo::Image img(3, 2);

    img.setPixel(2, 1, 2, 200);

    CHECK(img.getPixel(2, 1, 2) == 200);
    CHECK(img.getPixel(1, 1, 2) == 0);
}

// T03: qualquer coordenada fora da imagem 3x2 lança exceção.
void foraDosLimites() {
    modelo::Image img(3, 2);

    CHECK_THROWS(img.getPixel(3, 0, 0));
    CHECK_THROWS(img.getPixel(0, 2, 0));
    CHECK_THROWS(img.getPixel(0, 0, 3));
    CHECK_THROWS(img.getPixel(-1, 0, 0));
}

// T04: largura ou altura <= 0 lança exceção.
void dimensoesInvalidas() {
    CHECK_THROWS(modelo::Image(0, 5));
    CHECK_THROWS(modelo::Image(5, -1));
}

}  // namespace

void testesModelo() {
    dimensoes();
    gravarELer();
    foraDosLimites();
    dimensoesInvalidas();
}
