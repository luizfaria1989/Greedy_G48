#include "CommandLineApp.hpp"

#include "../algoritmos/extracao/ChannelExtractor.hpp"
#include "../algoritmos/extracao/PackedPixelExtractor.hpp"

#include "../algoritmos/predicao/LeftPredictor.hpp"
#include "../algoritmos/predicao/NoPredictor.hpp"
#include "../algoritmos/predicao/UpPredictor.hpp"
#include "../algoritmos/predicao/PaethPredictor.hpp"

#include "io/PPMFile.hpp"
#include "servico/Compressor.hpp"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace {

	using Clock = std::chrono::steady_clock;

	double secondsSince(Clock::time_point start) {
		return std::chrono::duration<double>(Clock::now() - start).count();
	}

	std::size_t fileSize(const std::string& path) {
		return static_cast<std::size_t>(std::filesystem::file_size(path));
	}

	bool sameImage(const modelo::Image& a, const modelo::Image& b) {
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
}

namespace app {

CommandLineApp::CommandLineApp(int argc, char** argv)
	: args(argc > 0 ? argv + 1 : argv, argv + argc) {
}

int CommandLineApp::run() {

	try {
		parseArguments();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		printUsage();
		return 1;
	}

	try {
		if (mode == "comprimir") {
			auto predictor = createPredictor(predictorName);
			auto extractor = createExtractor(extractorName);
			servico::Compressor compressor(predictor.get(), extractor.get());

			auto start = Clock::now();
			compressor.compress(inputPath, outputPath);
			double seconds = secondsSince(start);

			printStats(fileSize(inputPath), fileSize(outputPath), seconds);
		} else if (mode == "descomprimir") {
			servico::HeaderInfo info = servico::Compressor::readHeaderInfo(inputPath);

			auto predictor = createPredictor(info.predictorName);
			auto extractor = createExtractor(info.extractorName);
			servico::Compressor compressor(predictor.get(), extractor.get());

			auto start = Clock::now();
			compressor.decompress(inputPath, outputPath);
			double seconds = secondsSince(start);

			std::printf("Descomprimido em %.3f s\n", seconds);
		} else {
			runBenchmark();
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}

void CommandLineApp:: parseArguments() {

	if (args.empty()) {
		throw std::invalid_argument("No argument passed");
	}

	mode = args[0];

	std::size_t next = 0;

	if (mode == "benchmark") {
		if (args.size() < 2) {
			throw std::invalid_argument("Benchamark precisa dos argumentos de entrada");
		}
		inputPath = args[1];
		next = 2;
	} else if (mode == "comprimir" || mode == "descomprimir") {
		if (args.size() < 3) {
			throw std::invalid_argument(mode + "Precisa do arquivo de entrada e saida");
		}
		inputPath = args[1];
		outputPath = args[2];
		next = 3;
	} else {
		throw std::invalid_argument("Unknown mode");
	}

	for (std::size_t i = next; i < args.size(); ++i) {
		const std::string& option = args[i];
		bool hasValue = (i + 1 < args.size());

		if ((option == "--preditor" || option == "--predictor") && hasValue) {
			predictorName = args[++i];
		} else if ((option == "--extrator" || option == "--extractor") && hasValue) {
			extractorName = args[++i];
		} else {
			throw std::invalid_argument("Unknown option");
		}
	}

	createPredictor(predictorName);
	createExtractor(extractorName);

}

std::unique_ptr<algoritmos::predicao::Predictor> CommandLineApp::createPredictor(const std::string& name) const{

	using namespace algoritmos::predicao;

	// Aceita o nome curto da linha de comando e o nome da classe gravado no cabecalho do .huff.
	if (name == "nenhum" || name == "NoPredictor") return std::make_unique<NoPredictor>();
	if (name == "cima" || name == "UpPredictor") return std::make_unique<UpPredictor>();
	if (name == "esquerda" || name == "LeftPredictor") return std::make_unique<LeftPredictor>();
	if (name == "paeth" || name == "PaethPredictor") return std::make_unique<PaethPredictor>();

	throw std::invalid_argument("Unknown predictor name: " + name);

}

std::unique_ptr<algoritmos::extracao::SymbolExtractor> CommandLineApp::createExtractor(const std::string& name) const{

	using namespace algoritmos::extracao;

	if (name == "canal" || name == "ChannelExtractor") return std::make_unique<ChannelExtractor>();
	if (name == "pixel" || name == "PackedPixelExtractor") return std::make_unique<PackedPixelExtractor>();

	throw std::invalid_argument("Unknown extractor name: " + name);

}

void CommandLineApp::runBenchmark() const{

	const std::vector<std::string> predictor = {"NoPredictor", "UpPredictor", "LeftPredictor", "PaethPredictor",};
	const std::vector<std::string> extractor = {"ChannelExtractor", "PackedPixelExtractor",};

	const std::string huffPath = inputPath + ".bench.huff";
	const std::string ppmPath = inputPath + ".bench.ppm";

	modelo::Image original = io::PPMFile::read(inputPath);
	std::size_t originalSize = fileSize(inputPath);

	std::printf("%-16s %-22s %12s %10s %10s %10s %s\n", "preditor", "extrator", "comprimido", "taxa", "comp (s)", "desc (s)", "lossless");

	for (const std::string& p : predictor) {
		for (const std::string& e : extractor) {
			auto predictor = createPredictor(p);
			auto extractor = createExtractor(e);
			servico::Compressor compressor(predictor.get(), extractor.get());

			auto start = Clock::now();
			compressor.compress(inputPath, huffPath);
			double compressSeconds = secondsSince(start);

			start = Clock::now();
			compressor.decompress(huffPath, ppmPath);
			double decompressSeconds = secondsSince(start);

			std::size_t compressedSize = fileSize(huffPath);
			double ratio = 100.0 * static_cast<double>(compressedSize) / static_cast<double>(originalSize);
			bool ok = sameImage(original, io::PPMFile::read(ppmPath));

			std::printf("%-16s %-22s %12zu %9.2f%% %10.3f %10.3f %s\n", p.c_str(), e.c_str(), compressedSize, ratio, compressSeconds, decompressSeconds, ok ? "OK" : "FAILED");
		}
	}
	std::filesystem::remove(huffPath);
	std::filesystem::remove(ppmPath);

}

void CommandLineApp::printStats(std::size_t originalSize, std::size_t compressedSize, double seconds) const{
	double ratio = 100.0 * static_cast<double>(compressedSize) / static_cast<double>(originalSize);

	std::printf("Original: %zu bytes\n", originalSize);
	std::printf("Comprimido: %zu bytes\n", compressedSize);
	std::printf("Taxa: %.2f%% do original (economia de %.2f%%)\n", ratio, 100.0 - ratio);
	std::printf("Tempo: %.3f s\n", seconds);
}

void CommandLineApp::printUsage() const{
	std::fprintf(stderr,
		"Uso:\n"
		" huff comprimir <entrada.ppm> <saida.huff> [--preditor NOME] [--extrator NOME]\n"
		" huff descomprimir <entrada.huff> <saida.ppm>\n"
		" huff benchmark <entrada.ppm>\n"
		"\n"
		"Preditores: nenhum, esquerda, cima, paeth (padrao: paeth)\n"
		"Extratores: canal, pixel (padrao: canal)\n");
}
}  // namespace app
