#ifndef ALGORITMOS_EXTRACAO_SYMBOL_EXTRACTOR_H
#define ALGORITMOS_EXTRACAO_SYMBOL_EXTRACTOR_H

#include <string>
#include <vector>

#include "algoritmos/extracao/SymbolStream.hpp"
#include "modelo/Image.hpp"

namespace algoritmos
{
namespace extracao
{
class SymbolExtractor
{
public:
	virtual std::string getName()=0;

	virtual std::vector<SymbolStream> extract(modelo::Image img)=0;

	virtual modelo::Image rebuild(std::vector<SymbolStream> streams, int width, int height)=0;

	˜SymbolExtractor();

};

}  // namespace extracao
}  // namespace algoritmos
#endif
