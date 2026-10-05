#ifndef TESTS_TESTFRAMEWORK_HPP
#define TESTS_TESTFRAMEWORK_HPP

#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

#include "modelo/Image.hpp"

namespace testes {

// "inline" (C++17) permite definir a variável no header sem duplicar entre os .cpp.
inline int total = 0;
inline int falhas = 0;

// Imagem com valores "aleatórios", mas sempre os mesmos para a mesma semente.
inline modelo::Image imagemPseudoAleatoria(int width, int height, std::uint32_t semente) {
    modelo::Image img(width, height);
    std::uint32_t estado = semente;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < 3; ++c) {
                estado = estado * 1103515245u + 12345u;  // o rand() clássico do C
                img.setPixel(x, y, c, static_cast<std::uint8_t>(estado >> 16));
            }
        }
    }
    return img;
}

// Compara pixel a pixel.
inline bool mesmaImagem(const modelo::Image& a, const modelo::Image& b) {
    if (a.getWidth() != b.getWidth() || a.getHeight() != b.getHeight()) {
        return false;
    }
    for (int y = 0; y < a.getHeight(); ++y) {
        for (int x = 0; x < a.getWidth(); ++x) {
            for (int c = 0; c < 3; ++c) {
                if (a.getPixel(x, y, c) != b.getPixel(x, y, c)) {
                    return false;
                }
            }
        }
    }
    return true;
}

// Caminho na pasta temporária do sistema; o arquivo é apagado quando o objeto morre.
struct ArquivoTemporario {
    std::string path;

    explicit ArquivoTemporario(const std::string& nome)
        : path((std::filesystem::temp_directory_path() / nome).string()) {}
    ~ArquivoTemporario() {
        std::error_code ec;
        std::filesystem::remove(path, ec);
    }
    ArquivoTemporario(const ArquivoTemporario&) = delete;
    ArquivoTemporario& operator=(const ArquivoTemporario&) = delete;
};

}  // namespace testes

// Confere uma condição. #cond vira o texto da condição, como no C.
#define CHECK(cond)                                                     \
    do {                                                                \
        ++testes::total;                                                \
        if (!(cond)) {                                                  \
            ++testes::falhas;                                           \
            std::cerr << "  FALHOU: " << #cond                          \
                      << "  (" << __FILE__ << ":" << __LINE__ << ")\n"; \
        }                                                               \
    } while (0)

// Confere que a expressão lança uma exceção.
#define CHECK_THROWS(expr)                                              \
    do {                                                                \
        ++testes::total;                                                \
        bool lancou = false;                                            \
        try {                                                           \
            (void)(expr);                                               \
        } catch (const std::exception&) {                               \
            lancou = true;                                              \
        }                                                               \
        if (!lancou) {                                                  \
            ++testes::falhas;                                           \
            std::cerr << "  NAO LANCOU: " << #expr                      \
                      << "  (" << __FILE__ << ":" << __LINE__ << ")\n"; \
        }                                                               \
    } while (0)

#endif