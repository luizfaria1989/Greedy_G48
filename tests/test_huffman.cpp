#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "TestFramework.hpp"
#include "algoritmos/huffman/FrequencyTable.hpp"
#include "algoritmos/huffman/HuffmanNode.hpp"
#include "algoritmos/huffman/HuffmanTree.hpp"
#include "io/BitReader.hpp"
#include "io/BitWriter.hpp"

using algoritmos::extracao::SymbolStream;
using algoritmos::huffman::FrequencyTable;
using algoritmos::huffman::HuffmanNode;
using algoritmos::huffman::HuffmanTree;

namespace {

// A=65 (5 vezes), B=66 (2), C=67 (1), D=68 (1).
SymbolStream fluxoABCD() {
    SymbolStream s;
    s.insert(s.end(), 5, 65);
    s.insert(s.end(), 2, 66);
    s.push_back(67);
    s.push_back(68);
    return s;
}

// Fluxo fixo de 200 símbolos (0..255) com a mesma sequência a cada execução.
SymbolStream fluxoPseudoAleatorio() {
    SymbolStream s;
    std::uint32_t estado = 7;
    for (int i = 0; i < 200; ++i) {
        estado = estado * 1103515245u + 12345u;
        s.push_back((estado >> 16) & 0xFF);
    }
    return s;
}

// Número 0..255 do byte i.
int byteEm(const std::string& bytes, std::size_t i) {
    return static_cast<int>(static_cast<unsigned char>(bytes[i]));
}

// Grava a tabela e lê de volta.
FrequencyTable idaEVolta(const FrequencyTable& tabela) {
    std::stringstream ss;
    tabela.writeTo(ss);
    return FrequencyTable::readFrom(ss);
}

// Árvores da tabela original e da tabela lida do "arquivo" têm que dar os mesmos códigos.
bool mesmosCodigos(const SymbolStream& fluxo) {
    FrequencyTable original(fluxo);
    FrequencyTable lida = idaEVolta(original);
    HuffmanTree a(original);
    HuffmanTree b(lida);
    for (const auto& entrada : original.getCounts()) {
        if (!(a.getCode(entrada.first) == b.getCode(entrada.first))) {
            return false;
        }
    }
    return true;
}

// T17: nó interno soma as frequências e guarda os filhos na ordem.
void noInterno() {
    auto esquerda = std::make_unique<HuffmanNode>(65, 3);
    auto direita = std::make_unique<HuffmanNode>(66, 5);

    HuffmanNode pai(std::move(esquerda), std::move(direita));

    CHECK(pai.getFrequency() == 8);
    CHECK(!pai.isLeaf());
    CHECK(pai.getLeft() != nullptr && pai.getLeft()->getSymbol() == 65);
    CHECK(pai.getRight() != nullptr && pai.getRight()->getSymbol() == 66);
}

// T18: contagem por símbolo; símbolo ausente vale 0.
void contagens() {
    FrequencyTable tabela(SymbolStream{5, 5, 7});

    CHECK(tabela.getFrequency(5) == 2);
    CHECK(tabela.getFrequency(7) == 1);
    CHECK(tabela.getFrequency(9) == 0);
}

// T19: total de símbolos e quantidade de símbolos distintos.
void total() {
    FrequencyTable tabela(SymbolStream{5, 5, 7});

    CHECK(tabela.getTotal() == 3);
    CHECK(tabela.getCounts().size() == 2);
}

// T20: a tabela gravada e lida tem as mesmas frequências e o mesmo total.
void tabelaIdaEVolta() {
    FrequencyTable original(fluxoABCD());

    FrequencyTable lida = idaEVolta(original);

    CHECK(lida.getFrequency(65) == 5);
    CHECK(lida.getFrequency(66) == 2);
    CHECK(lida.getFrequency(67) == 1);
    CHECK(lida.getFrequency(68) == 1);
    CHECK(lida.getTotal() == original.getTotal());
    CHECK(lida.getCounts() == original.getCounts());
}

// T21: tabela cortada no meio lança exceção ao ler.
void tabelaTruncada() {
    FrequencyTable tabela(fluxoABCD());
    std::stringstream ss;
    tabela.writeTo(ss);
    std::string bytes = ss.str();
    std::istringstream cortado(bytes.substr(0, bytes.size() - 3));

    CHECK_THROWS(FrequencyTable::readFrom(cortado));
}

// T22: códigos de A=5, B=2, C=1, D=1 calculados à mão: A=1, B=00, C=010, D=011.
void codigosABCD() {
    FrequencyTable tabela(fluxoABCD());

    HuffmanTree tree(tabela);

    CHECK((tree.getCode(65) == std::vector<bool>{true}));
    CHECK((tree.getCode(66) == std::vector<bool>{false, false}));
    CHECK((tree.getCode(67) == std::vector<bool>{false, true, false}));
    CHECK((tree.getCode(68) == std::vector<bool>{false, true, true}));
}

// T23: 1 00 010 011 = 10001001 (137) + 1 + zeros = 10000000 (128); decodifica A, B, C, D.
void abcdDePontaAPonta() {
    FrequencyTable tabela(fluxoABCD());
    HuffmanTree tree(tabela);
    std::ostringstream out;
    io::BitWriter writer(out);

    for (std::uint32_t simbolo : {65u, 66u, 67u, 68u}) {
        writer.writeCode(tree.getCode(simbolo));
    }
    writer.flush();

    std::string bytes = out.str();
    CHECK(bytes.size() == 2);
    CHECK(bytes.size() == 2 && byteEm(bytes, 0) == 137);
    CHECK(bytes.size() == 2 && byteEm(bytes, 1) == 128);

    std::istringstream in(bytes);
    io::BitReader reader(in);
    CHECK(tree.decodeSymbol(reader) == 65);
    CHECK(tree.decodeSymbol(reader) == 66);
    CHECK(tree.decodeSymbol(reader) == 67);
    CHECK(tree.decodeSymbol(reader) == 68);
}

// T24: com um símbolo só, o código é "0" (1 bit) e cada decodeSymbol consome 1 bit.
void simboloUnico() {
    FrequencyTable tabela(SymbolStream{9, 9, 9});
    HuffmanTree tree(tabela);
    std::ostringstream out;
    io::BitWriter writer(out);

    CHECK((tree.getCode(9) == std::vector<bool>{false}));
    for (int i = 0; i < 3; ++i) {
        writer.writeCode(tree.getCode(9));
    }
    writer.flush();

    std::string bytes = out.str();
    CHECK(bytes.size() == 1);
    CHECK(bytes.size() == 1 && byteEm(bytes, 0) == 0);

    std::istringstream in(bytes);
    io::BitReader reader(in);
    for (int i = 0; i < 3; ++i) {
        CHECK(tree.decodeSymbol(reader) == 9);
    }
}

// T25: a árvore feita da tabela lida do arquivo tem os mesmos códigos da original.
void determinismo() {
    CHECK(mesmosCodigos(fluxoABCD()));
    CHECK(mesmosCodigos(fluxoPseudoAleatorio()));
}

// T26: símbolo que não está na tabela não tem código.
void simboloAusente() {
    FrequencyTable tabela(fluxoABCD());
    HuffmanTree tree(tabela);

    CHECK_THROWS(tree.getCode(90));
}

}  // namespace

void testesHuffman() {
    noInterno();
    contagens();
    total();
    tabelaIdaEVolta();
    tabelaTruncada();
    codigosABCD();
    abcdDePontaAPonta();
    simboloUnico();
    determinismo();
    simboloAusente();
}
