#ifndef APP_COMMAND_LINE_APP_H
#define APP_COMMAND_LINE_APP_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "../algoritmos/extracao/SymbolExtractor.hpp"
#include "../algoritmos/predicao/Predictor.hpp"

namespace app {
class CommandLineApp {

public:
	CommandLineApp(int argc, char** argv);

	int run();

private:

	void  parseArguments();

	std::unique_ptr<algoritmos::predicao::Predictor> createPredictor(const std::string& name) const;

	std::unique_ptr<algoritmos::extracao::SymbolExtractor> createExtractor(const std::string& name) const;

	void runBenchmark() const;

	void printStats(size_t originalSize, size_t compressedSize, double seconds) const;

	void printUsage() const;

	std::vector<std::string> args;
	std::string mode;
	std::string inputPath;
	std::string outputPath;
	std::string predictorName = "PaethPredictor";
	std::string extractorName = "ChannelExtractor";

};

}  // namespace app
#endif
