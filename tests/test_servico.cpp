#include <string>

#include "TestFramework.hpp"
#include "algoritmos/extracao/ChannelExtractor.hpp"
#include "algoritmos/extracao/PackedPixelExtractor.hpp"
#include "algoritmos/predicao/LeftPredictor.hpp"
#include "algoritmos/predicao/NoPredictor.hpp"
#include "algoritmos/predicao/PaethPredictor.hpp"
#include "algoritmos/predicao/UpPredictor.hpp"
#include "io/PPMFile.hpp"
#include "servico/Compressor.hpp"

using namespace algoritmos::extracao;
using namespace algoritmos::predicao;
using servico::Compressor;

namespace {

// compress -> decompress -> read; devolve true se a imagem voltou idêntica.
// "id" deixa os nomes dos arquivos temporários únicos por chamada.
bool voltaIgual(const modelo::Image& original, Predictor* preditor, SymbolExtractor* extrator,
                const std::string& id) {
    testes::ArquivoTemporario ppm("huff_teste_" + id + ".ppm");
    testes::ArquivoTemporario huff("huff_teste_" + id + ".huff");
    testes::ArquivoTemporario saida("huff_teste_" + id + ".saida.ppm");
    io::PPMFile::write(ppm.path, original);

    Compressor compressor(preditor, extrator);
    compressor.compress(ppm.path, huff.path);
    compressor.decompress(huff.path, saida.path);

    return testes::mesmaImagem(original, io::PPMFile::read(saida.path));
}

// T40: os quatro preditores x os dois extratores.
void oitoCombinacoes() {
    modelo::Image img = testes::imagemPseudoAleatoria(9, 7, 4);
    NoPredictor nenhum;
    LeftPredictor esquerda;
    UpPredictor cima;
    PaethPredictor paeth;
    ChannelExtractor canal;
    PackedPixelExtractor pixel;
    Predictor* preditores[] = {&nenhum, &esquerda, &cima, &paeth};
    SymbolExtractor* extratores[] = {&canal, &pixel};

    int n = 0;
    for (Predictor* p : preditores) {
        for (SymbolExtractor* e : extratores) {
            CHECK(voltaIgual(img, p, e, "t40_" + std::to_string(n++)));
        }
    }
}

// T41: imagem 1x1 e imagem 16x16 toda (0, 0, 255), com paeth+canal e nenhum+pixel.
void casosDeBorda() {
    modelo::Image minima = testes::imagemPseudoAleatoria(1, 1, 6);
    modelo::Image azul(16, 16);
    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            azul.setPixel(x, y, 2, 255);
        }
    }
    PaethPredictor paeth;
    NoPredictor nenhum;
    ChannelExtractor canal;
    PackedPixelExtractor pixel;

    CHECK(voltaIgual(minima, &paeth, &canal, "t41_a"));
    CHECK(voltaIgual(minima, &nenhum, &pixel, "t41_b"));
    CHECK(voltaIgual(azul, &paeth, &canal, "t41_c"));
    CHECK(voltaIgual(azul, &nenhum, &pixel, "t41_d"));
}

// T42: o cabeçalho do .huff guarda dimensões, número de fluxos e os nomes.
void cabecalho() {
    testes::ArquivoTemporario ppm("huff_teste_t42.ppm");
    testes::ArquivoTemporario huff("huff_teste_t42.huff");
    io::PPMFile::write(ppm.path, testes::imagemPseudoAleatoria(5, 3, 8));
    LeftPredictor esquerda;
    PackedPixelExtractor pixel;

    Compressor(&esquerda, &pixel).compress(ppm.path, huff.path);
    servico::HeaderInfo info = Compressor::readHeaderInfo(huff.path);

    CHECK(info.width == 5);
    CHECK(info.height == 3);
    CHECK(info.numStreams == 1);
    CHECK(info.predictorName == "LeftPredictor");
    CHECK(info.extractorName == "PackedPixelExtractor");
}

// T43: um .ppm não é um .huff.
void arquivoQueNaoEHuff() {
    testes::ArquivoTemporario ppm("huff_teste_t43.ppm");
    testes::ArquivoTemporario saida("huff_teste_t43.saida.ppm");
    io::PPMFile::write(ppm.path, testes::imagemPseudoAleatoria(4, 4, 9));
    PaethPredictor paeth;
    ChannelExtractor canal;
    Compressor compressor(&paeth, &canal);

    CHECK_THROWS(Compressor::readHeaderInfo(ppm.path));
    CHECK_THROWS(compressor.decompress(ppm.path, saida.path));
}

// T44: descomprimir com outro preditor que o do arquivo é erro, não lixo.
void preditorTrocado() {
    testes::ArquivoTemporario ppm("huff_teste_t44.ppm");
    testes::ArquivoTemporario huff("huff_teste_t44.huff");
    testes::ArquivoTemporario saida("huff_teste_t44.saida.ppm");
    io::PPMFile::write(ppm.path, testes::imagemPseudoAleatoria(6, 6, 10));
    PaethPredictor paeth;
    LeftPredictor esquerda;
    ChannelExtractor canal;
    Compressor(&paeth, &canal).compress(ppm.path, huff.path);

    Compressor errado(&esquerda, &canal);

    CHECK_THROWS(errado.decompress(huff.path, saida.path));
}

// T45: preditor ou extrator nulo.
void ponteiroNulo() {
    PaethPredictor paeth;
    ChannelExtractor canal;

    CHECK_THROWS(Compressor(nullptr, &canal));
    CHECK_THROWS(Compressor(&paeth, nullptr));
}

}  // namespace

void testesServico() {
    oitoCombinacoes();
    casosDeBorda();
    cabecalho();
    arquivoQueNaoEHuff();
    preditorTrocado();
    ponteiroNulo();
}
