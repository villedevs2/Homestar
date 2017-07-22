// Windows specific implementation of BinaryFile class

#pragma once

#define BOOST_DATE_TIME_NO_LIB

#include <fstream>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <string>
#include <format/format.h>

namespace Debug
{
	void start();
	void end();
	void log(std::string string);
	void error(std::string string);

	static std::fstream m_log;
};
