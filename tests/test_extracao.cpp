#include <vector>

#include "TestFramework.hpp"
#include "algoritmos/extracao/ChannelExtractor.hpp"
#include "algoritmos/extracao/PackedPixelExtractor.hpp"

using algoritmos::extracao::ChannelExtractor;
using algoritmos::extracao::PackedPixelExtractor;
using algoritmos::extracao::SymbolStream;

namespace {

// Imagem 2x1 com os pixels (10, 20, 30) e (40, 50, 60).
modelo::Image imagemBase() {
    modelo::Image img(2, 1);
    const int valores[6] = {10, 20, 30, 40, 50, 60};
    for (int x = 0; x < 2; ++x) {
        for (int c = 0; c < 3; ++c) {
            img.setPixel(x, 0, c, static_cast<std::uint8_t>(valores[x * 3 + c]));
        }
    }
    return img;
}

// T27: um fluxo por canal: R, G, B.
void fluxosPorCanal() {
    auto streams = ChannelExtractor().extract(imagemBase());

    CHECK(streams.size() == 3);
    CHECK(streams.size() == 3 && streams[0] == SymbolStream({10, 40}));
    CHECK(streams.size() == 3 && streams[1] == SymbolStream({20, 50}));
    CHECK(streams.size() == 3 && streams[2] == SymbolStream({30, 60}));
}

// T28: extract e rebuild do canal devolvem a imagem original.
void canalIdaEVolta() {
    ChannelExtractor extrator;
    modelo::Image original = testes::imagemPseudoAleatoria(5, 4, 2);

    modelo::Image refeita = extrator.rebuild(extrator.extract(original), 5, 4);

    CHECK(testes::mesmaImagem(original, refeita));
}

// T29: pixel empacotado = R*65536 + G*256 + B. (10,20,30) = 660510; (40,50,60) = 2634300.
void pixelEmpacotado() {
    auto streams = PackedPixelExtractor().extract(imagemBase());

    CHECK(streams.size() == 1);
    CHECK(streams.size() == 1 && streams[0] == SymbolStream({660510, 2634300}));
}

// T30: extract e rebuild do pixel empacotado devolvem a imagem original.
void pixelIdaEVolta() {
    PackedPixelExtractor extrator;
    modelo::Image original = testes::imagemPseudoAleatoria(5, 4, 2);

    modelo::Image refeita = extrator.rebuild(extrator.extract(original), 5, 4);

    CHECK(testes::mesmaImagem(original, refeita));
}

// T31: rebuild rejeita número de fluxos errado, tamanho errado e símbolo grande demais.
void entradasInvalidas() {
    ChannelExtractor canal;
    PackedPixelExtractor pixel;
    std::vector<SymbolStream> doisFluxos = {{1, 2}, {1, 2}};
    std::vector<SymbolStream> tamanhoErrado = {{1, 2}, {1, 2}, {1}};
    std::vector<SymbolStream> simbolo300 = {{1, 2}, {300, 2}, {1, 2}};
    std::vector<SymbolStream> simboloGrande = {{0x1000000, 0}};

    CHECK_THROWS(canal.rebuild(doisFluxos, 2, 1));
    CHECK_THROWS(canal.rebuild(tamanhoErrado, 2, 1));
    CHECK_THROWS(canal.rebuild(simbolo300, 2, 1));
    CHECK_THROWS(pixel.rebuild(simboloGrande, 2, 1));
}

// T32: nomes usados no cabeçalho do .huff e na fábrica do CommandLineApp.
void nomes() {
    CHECK(ChannelExtractor().getName() == "ChannelExtractor");
    CHECK(PackedPixelExtractor().getName() == "PackedPixelExtractor");
}

}  // namespace

void testesExtracao() {
    fluxosPorCanal();
    canalIdaEVolta();
    pixelEmpacotado();
    pixelIdaEVolta();
    entradasInvalidas();
    nomes();
}
