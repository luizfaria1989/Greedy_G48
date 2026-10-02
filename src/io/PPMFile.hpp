#ifndef IO_P_P_M_FILE_H
#define IO_P_P_M_FILE_H

#include <string>

#include "../modelo/Image.hpp"

namespace io
{
class PPMFile
{
public:
	static modelo::Image read(std::string path);

	static void write(std::string path, modelo::Image img);

};

}  // namespace io
#endif
