#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "TestFramework.hpp"
#include "io/BitReader.hpp"
#include "io/BitWriter.hpp"
#include "io/PPMFile.hpp"

namespace {

// Valor de 0 a 255 do byte i (char tem sinal no macOS: 160 viraria -96).
int byteEm(const std::string& bytes, std::size_t i) {
    return static_cast<int>(static_cast<unsigned char>(bytes[i]));
}

// Monta um arquivo à mão: o cabeçalho como texto e depois os bytes dos pixels.
void escreverPPM(const std::string& path, const std::string& cabecalho,
                 const std::vector<int>& pixels) {
    std::ofstream out(path, std::ios::binary);
    out << cabecalho;
    for (int b : pixels) {
        out.put(static_cast<char>(b));
    }
}

// T05: gravar e ler devolve a mesma imagem.
void ppmIdaEVolta() {
    testes::ArquivoTemporario arquivo("huff_teste_t05.ppm");
    modelo::Image original = testes::imagemPseudoAleatoria(4, 3, 1);

    io::PPMFile::write(arquivo.path, original);
    modelo::Image lida = io::PPMFile::read(arquivo.path);

    CHECK(testes::mesmaImagem(original, lida));
}

// T06: o comentário "# ..." no cabeçalho é ignorado.
void ppmComComentario() {
    testes::ArquivoTemporario arquivo("huff_teste_t06.ppm");
    escreverPPM(arquivo.path, "P6\n# Created by GIMP\n2 1\n255\n", {10, 20, 30, 40, 50, 60});

    modelo::Image img = io::PPMFile::read(arquivo.path);

    CHECK(img.getWidth() == 2);
    CHECK(img.getHeight() == 1);
    CHECK(img.getPixel(1, 0, 1) == 50);
}

// T07: pixels que começam com bytes de "espaço" (10 = \n, 32 = ' ') não podem ser pulados.
void ppmPrimeiroByteEspaco() {
    testes::ArquivoTemporario arquivo("huff_teste_t07.ppm");
    escreverPPM(arquivo.path, "P6\n# Created by GIMP\n2 1\n255\n", {10, 32, 30, 40, 50, 60});

    modelo::Image img = io::PPMFile::read(arquivo.path);

    CHECK(img.getPixel(0, 0, 0) == 10);
    CHECK(img.getPixel(0, 0, 1) == 32);
}

// T08: P3, valor máximo 65535 e pixels faltando são rejeitados.
void ppmInvalidos() {
    testes::ArquivoTemporario p3("huff_teste_t08_p3.ppm");
    testes::ArquivoTemporario max16("huff_teste_t08_max.ppm");
    testes::ArquivoTemporario curto("huff_teste_t08_curto.ppm");
    escreverPPM(p3.path, "P3\n2 1\n255\n", {10, 20, 30, 40, 50, 60});
    escreverPPM(max16.path, "P6\n2 1\n65535\n", {10, 20, 30, 40, 50, 60});
    escreverPPM(curto.path, "P6\n2 1\n255\n", {10, 20, 30, 40, 50});

    CHECK_THROWS(io::PPMFile::read(p3.path));
    CHECK_THROWS(io::PPMFile::read(max16.path));
    CHECK_THROWS(io::PPMFile::read(curto.path));
}

// T09: os bits 1, 0, 1 viram o byte 10100000 = 160.
void bitWriterUmZeroUm() {
    // Preparar
    std::ostringstream out;
    io::BitWriter writer(out);

    // Executar
    writer.writeBit(true);
    writer.writeBit(false);
    writer.writeBit(true);
    writer.flush();

    // Verificar
    std::string bytes = out.str();
    CHECK(bytes.size() == 1);
    CHECK(bytes.size() == 1 && byteEm(bytes, 0) == 0b10100000);
}

// T10: oito bits completam um byte sozinhos, sem flush.
void bitWriterByteCheio() {
    std::ostringstream out;
    io::BitWriter writer(out);

    for (int i = 0; i < 8; ++i) {
        writer.writeBit(true);
    }

    std::string bytes = out.str();
    CHECK(bytes.size() == 1);
    CHECK(bytes.size() == 1 && byteEm(bytes, 0) == 255);
}

// T11: nove 1s e flush = 11111111 e 10000000.
void bitWriterNoveBits() {
    std::ostringstream out;
    io::BitWriter writer(out);

    for (int i = 0; i < 9; ++i) {
        writer.writeBit(true);
    }
    writer.flush();

    std::string bytes = out.str();
    CHECK(bytes.size() == 2);
    CHECK(bytes.size() == 2 && byteEm(bytes, 0) == 255);
    CHECK(bytes.size() == 2 && byteEm(bytes, 1) == 128);
}

// T12: flush sem nenhum bit não grava nada.
void bitWriterFlushVazio() {
    std::ostringstream out;
    io::BitWriter writer(out);

    writer.flush();

    CHECK(out.str().empty());
}

// T13: o byte 160 é lido como 1, 0, 1, 0, 0, 0, 0, 0.
void bitReaderLe160() {
    std::istringstream in(std::string(1, static_cast<char>(160)));
    io::BitReader reader(in);

    const bool esperado[8] = {true, false, true, false, false, false, false, false};
    for (int i = 0; i < 8; ++i) {
        CHECK(reader.readBit() == esperado[i]);
    }
}

// T14: 13 bits gravados voltam na mesma ordem.
void bitsIdaEVolta() {
    const std::vector<bool> bits = {true, false, true, true, false, false, true,
                                    false, true, true, true, false, true};
    std::ostringstream out;
    io::BitWriter writer(out);
    for (bool b : bits) {
        writer.writeBit(b);
    }
    writer.flush();

    std::istringstream in(out.str());
    io::BitReader reader(in);
    std::vector<bool> lidos;
    for (std::size_t i = 0; i < bits.size(); ++i) {
        lidos.push_back(reader.readBit());
    }

    CHECK((lidos == bits));
}

// T15: depois de 3 bits + flush, alignToByte pula o resto do byte e o próximo byte vem inteiro.
void alinhamento() {
    std::ostringstream out;
    io::BitWriter writer(out);
    writer.writeBit(true);
    writer.writeBit(false);
    writer.writeBit(true);
    writer.flush();
    const bool segundo[8] = {true, false, true, true, false, false, true, true};
    for (bool b : segundo) {
        writer.writeBit(b);
    }
    writer.flush();

    std::istringstream in(out.str());
    io::BitReader reader(in);
    for (int i = 0; i < 3; ++i) {
        reader.readBit();
    }
    reader.alignToByte();

    for (int i = 0; i < 8; ++i) {
        CHECK(reader.readBit() == segundo[i]);
    }
}

// T16: ler de um fluxo vazio lança exceção.
void fimDeArquivo() {
    std::istringstream in("");
    io::BitReader reader(in);

    CHECK_THROWS(reader.readBit());
}

}  // namespace

void testesIO() {
    ppmIdaEVolta();
    ppmComComentario();
    ppmPrimeiroByteEspaco();
    ppmInvalidos();
    bitWriterUmZeroUm();
    bitWriterByteCheio();
    bitWriterNoveBits();
    bitWriterFlushVazio();
    bitReaderLe160();
    bitsIdaEVolta();
    alinhamento();
    fimDeArquivo();
}
