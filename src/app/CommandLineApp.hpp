#ifndef APP_COMMAND_LINE_APP_H
#define APP_COMMAND_LINE_APP_H

#include <string>
#include <vector>

#include "../algoritmos/extracao/SymbolExtractor.hpp"
#include "../algoritmos/predicao/Predictor.hpp"

namespace app
{
class CommandLineApp
{
private:
	std::vector<std::string> args;

	std::string mode;

	std::string inputPath;

	std::string outputPath;

	std::string predictorName;

	std::string extractorName;


private:
	void  parseArguments();

	std::unique_ptr<algoritmos::predicao::Predictor> createPredictor(std::string name);

	std::unique_ptr<algoritmos::extracao::SymbolExtractor> createExtractor(std::string name);

	void runBenchmark();

	void printStats(size_t originalSize, size_t compressedSize, double seconds);

	void printUsage();

public:
	CommandLineApp(int argc, char** argv);

	int run();

};

}  // namespace app
#endif
