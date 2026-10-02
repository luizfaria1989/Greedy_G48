#ifndef IO_P_P_M_FILE_H
#define IO_P_P_M_FILE_H

#include <string>
#include <istream>
#include "modelo/Image.hpp"

namespace io {

class PPMFile {

public:

	PPMFile() = delete;

	static modelo::Image read(const std::string& path);
	static void write(const std::string& path, const modelo::Image& img);

private:

	static int readHeaderValue(std::istream& in);

};

}  // namespace io
#endif
