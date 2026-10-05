#include <memory>
#include <vector>

#include "TestFramework.hpp"
#include "algoritmos/predicao/LeftPredictor.hpp"
#include "algoritmos/predicao/NoPredictor.hpp"
#include "algoritmos/predicao/PaethPredictor.hpp"
#include "algoritmos/predicao/UpPredictor.hpp"

using namespace algoritmos::predicao;

namespace {

// Imagem só com o canal vermelho preenchido (os outros ficam em 0), na ordem y, x.
modelo::Image imagemVermelha(int width, int height, const std::vector<int>& vermelho) {
    modelo::Image img(width, height);
    std::size_t i = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            img.setPixel(x, y, 0, static_cast<std::uint8_t>(vermelho[i++]));
        }
    }
    return img;
}

// T33: resíduo = valor - pixel da esquerda. 10, 12-10, 15-12.
void residuosEsquerda() {
    modelo::Image img = imagemVermelha(3, 1, {10, 12, 15});

    modelo::Image res = LeftPredictor().encode(img);

    CHECK(res.getPixel(0, 0, 0) == 10);
    CHECK(res.getPixel(1, 0, 0) == 2);
    CHECK(res.getPixel(2, 0, 0) == 3);
}

// T34: 90 - 100 = -10, que em módulo 256 vira 246; decode volta ao valor original.
void voltaDoModulo256() {
    modelo::Image img = imagemVermelha(2, 1, {100, 90});
    LeftPredictor preditor;

    modelo::Image res = preditor.encode(img);
    modelo::Image volta = preditor.decode(res);

    CHECK(res.getPixel(0, 0, 0) == 100);
    CHECK(res.getPixel(1, 0, 0) == 246);
    CHECK(volta.getPixel(0, 0, 0) == 100);
    CHECK(volta.getPixel(1, 0, 0) == 90);
}

// T35: sem predição, os resíduos são a própria imagem.
void semPredicao() {
    modelo::Image img = testes::imagemPseudoAleatoria(6, 4, 5);

    modelo::Image res = NoPredictor().encode(img);

    CHECK(testes::mesmaImagem(img, res));
}

// T36: resíduo = valor - pixel de cima, numa coluna 1x3.
void residuosCima() {
    modelo::Image img = imagemVermelha(1, 3, {10, 12, 15});

    modelo::Image res = UpPredictor().encode(img);

    CHECK(res.getPixel(0, 0, 0) == 10);
    CHECK(res.getPixel(0, 1, 0) == 2);
    CHECK(res.getPixel(0, 2, 0) == 3);
}

// T37: Paeth à mão. Em (1,1): esquerda=30, cima=20, canto=10; estimativa 30+20-10 = 40;
// o mais próximo é a esquerda (30), então o resíduo é 35-30 = 5.
void paethAMao() {
    modelo::Image img = imagemVermelha(2, 2, {10, 20, 30, 35});

    modelo::Image res = PaethPredictor().encode(img);

    CHECK(res.getPixel(0, 0, 0) == 10);
    CHECK(res.getPixel(1, 0, 0) == 10);
    CHECK(res.getPixel(0, 1, 0) == 20);
    CHECK(res.getPixel(1, 1, 0) == 5);
}

// T38: decode(encode(img)) devolve a imagem, nos quatro preditores (7x5: tamanhos ímpares).
void idaEVoltaDosQuatro() {
    modelo::Image img = testes::imagemPseudoAleatoria(7, 5, 3);
    NoPredictor nenhum;
    LeftPredictor esquerda;
    UpPredictor cima;
    PaethPredictor paeth;
    const Predictor* preditores[] = {&nenhum, &esquerda, &cima, &paeth};

    for (const Predictor* p : preditores) {
        CHECK(testes::mesmaImagem(img, p->decode(p->encode(img))));
    }
}

// T39: nomes usados no cabeçalho do .huff e na fábrica do CommandLineApp.
void nomes() {
    CHECK(NoPredictor().getName() == "NoPredictor");
    CHECK(LeftPredictor().getName() == "LeftPredictor");
    CHECK(UpPredictor().getName() == "UpPredictor");
    CHECK(PaethPredictor().getName() == "PaethPredictor");
}

}  // namespace

void testesPredicao() {
    residuosEsquerda();
    voltaDoModulo256();
    semPredicao();
    residuosCima();
    paethAMao();
    idaEVoltaDosQuatro();
    nomes();
}
