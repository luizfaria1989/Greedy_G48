cmake_minimum_required(VERSION 4.1)
project(Greedy_G48)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)
include_directories(src/algoritmos)
include_directories(src/algoritmos/extracao)
include_directories(src/algoritmos/huffman)
include_directories(src/algoritmos/predicao)
include_directories(src/app)
include_directories(src/io)
include_directories(src/modelo)
include_directories(src/servico)

add_executable(Greedy_G48
        src/algoritmos/extracao/ChannelExtractor.cpp
        src/algoritmos/extracao/ChannelExtractor.hpp
        src/algoritmos/extracao/PackedPixelExtractor.cpp
        src/algoritmos/extracao/PackedPixelExtractor.hpp
        src/algoritmos/extracao/SymbolExtractor.hpp
        src/algoritmos/extracao/SymbolStream.hpp
        src/algoritmos/huffman/FrequencyTable.cpp
        src/algoritmos/huffman/FrequencyTable.hpp
        src/algoritmos/huffman/HuffmanNode.cpp
        src/algoritmos/huffman/HuffmanNode.hpp
        src/algoritmos/huffman/HuffmanTree.cpp
        src/algoritmos/huffman/HuffmanTree.hpp
        src/algoritmos/predicao/LeftPredictor.hpp
        src/algoritmos/predicao/NoPredictor.hpp
        src/algoritmos/predicao/PaethPredictor.cpp
        src/algoritmos/predicao/PaethPredictor.hpp
        src/algoritmos/predicao/Predictor.cpp
        src/algoritmos/predicao/Predictor.hpp
        src/algoritmos/predicao/UpPredictor.hpp
        src/app/CommandLineApp.cpp
        src/app/CommandLineApp.hpp
        src/io/BitReader.cpp
        src/io/BitReader.hpp
        src/io/BitWriter.cpp
        src/io/BitWriter.hpp
        src/io/PPMFile.cpp
        src/io/PPMFile.hpp
        src/modelo/Image.cpp
        src/modelo/Image.hpp
        src/servico/Compressor.cpp
        src/servico/Compressor.hpp
        src/servico/HeaderInfo.hpp
        src/main.cpp)
