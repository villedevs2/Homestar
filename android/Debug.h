// Android specific implementation of BinaryFile class

#pragma once

#include <android/log.h>
#include <string>
#include <fstream>

namespace Debug
{
	void start();
	void end();
	void log(std::string string);
	void error(std::string string);

	static std::ofstream ofs;
};
