#include <exception>
#include <iostream>

#include "TestFramework.hpp"

// Cada arquivo test_*.cpp define uma destas funções.
void testesModelo();
void testesIO();
void testesHuffman();
void testesExtracao();
void testesPredicao();
void testesServico();

namespace {

// Roda uma suíte. Uma exceção inesperada vira falha, e os testes continuam.
void rodar(const char* nome, void (*suite)()) {
    std::cout << "[" << nome << "]\n";
    int antes = testes::falhas;
    try {
        suite();
    } catch (const std::exception& e) {
        ++testes::falhas;
        std::cerr << "  EXCECAO INESPERADA: " << e.what() << '\n';
    }
    std::cout << (testes::falhas == antes ? "  ok\n" : "  com falhas\n");
}

}  // namespace

int main() {
    rodar("modelo", testesModelo);
    rodar("io", testesIO);
    rodar("huffman", testesHuffman);
    rodar("extracao", testesExtracao);
    rodar("predicao", testesPredicao);
    rodar("servico", testesServico);

    std::cout << '\n' << (testes::total - testes::falhas) << '/' << testes::total
              << " verificacoes passaram\n";
    return testes::falhas == 0 ? 0 : 1;
}
