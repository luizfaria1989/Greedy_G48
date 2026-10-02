#include <string>
#include <vector>
#include <list>

#include "CommandLineApp.hpp"

namespace app
{

CommandLineApp::CommandLineApp(int argc, char** argv)
{
}

int CommandLineApp::run()
{
	return 0;
}

void CommandLineApp:: parseArguments()
{
}

std::unique_ptr<algoritmos::predicao::Predictor> CommandLineApp::createPredictor(std::string name)
{
	return 0;
}

std::unique_ptr<algoritmos::extracao::SymbolExtractor> CommandLineApp::createExtractor(std::string name)
{
	return 0;
}

void CommandLineApp::runBenchmark()
{
}

void CommandLineApp::printStats(size_t originalSize, size_t compressedSize, double seconds)
{
}

void CommandLineApp::printUsage()
{
}
}  // namespace app
}  // namespace 03_Modelo
