#ifndef ALGORITMOS_EXTRACAO_PACKED_PIXEL_EXTRACTOR_H
#define ALGORITMOS_EXTRACAO_PACKED_PIXEL_EXTRACTOR_H

#include <string>
#include <vector>

#include "algoritmos/extracao/SymbolExtractor.hpp"
#include "algoritmos/extracao/SymbolStream.hpp"
#include "modelo/Image.hpp"

namespace algoritmos
{
namespace extracao
{
class PackedPixelExtractor : public SymbolExtractor
{
public:
	std::string getName();

	std::vector<SymbolStream> extract(modelo::Image img);

	modelo::Image rebuild(std::vector<SymbolStream> streams, int width, int height);

};

}  // namespace extracao
}  // namespace algoritmos
#endif
