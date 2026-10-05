cmake_minimum_required(VERSION 3.16)
project(Greedy_G48 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Todas as classes do projeto, menos o main.cpp
add_library(huff_core STATIC
        src/modelo/Image.cpp
        src/io/PPMFile.cpp
        src/io/BitWriter.cpp
        src/io/BitReader.cpp
        src/algoritmos/huffman/HuffmanNode.cpp
        src/algoritmos/huffman/FrequencyTable.cpp
        src/algoritmos/huffman/HuffmanTree.cpp
        src/algoritmos/extracao/ChannelExtractor.cpp
        src/algoritmos/extracao/PackedPixelExtractor.cpp
        src/algoritmos/predicao/Predictor.cpp
        src/algoritmos/predicao/PaethPredictor.cpp
        src/servico/Compressor.cpp
        src/app/CommandLineApp.cpp
)
target_include_directories(huff_core PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_compile_options(huff_core PUBLIC -Wall -Wextra -Wpedantic)

# AddressSanitizer + UndefinedBehaviorSanitizer, só no perfil Debug
target_compile_options(huff_core PUBLIC
        $<$<CONFIG:Debug>:-fsanitize=address,undefined -fno-omit-frame-pointer>)
target_link_options(huff_core PUBLIC
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>)

# O programa
add_executable(huff src/main.cpp)
target_link_libraries(huff PRIVATE huff_core)

# Os testes
add_executable(huff_tests
        tests/test_main.cpp
        tests/test_modelo.cpp
        tests/test_io.cpp
        tests/test_huffman.cpp
        tests/test_extracao.cpp
        tests/test_predicao.cpp
        tests/test_servico.cpp
)
target_include_directories(huff_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(huff_tests PRIVATE huff_core)

enable_testing()
add_test(NAME huff_tests COMMAND huff_tests)