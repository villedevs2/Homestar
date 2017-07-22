// Windows specific implementation of Debug class

#include "Debug.h"

using namespace std;
using namespace boost::posix_time;

void Debug::start()
{
	m_log.open("log.txt", ios_base::out);
}

void Debug::end()
{
	m_log.close();
}

void Debug::log(std::string string)
{
	ptime timestamp = ptime(microsec_clock::local_time());
	time_duration td = timestamp.time_of_day();

	// timestamp
	m_log << str(fmt::Format("[{:=2d}:{:=2d}:{:=2d}] ") << td.hours() << td.minutes() << td.seconds());

	// debug string
	m_log << string << "\n";
}

void Debug::error(std::string string)
{
	ptime timestamp = ptime(microsec_clock::local_time());
	time_duration td = timestamp.time_of_day();

	// timestamp
	m_log << str(fmt::Format("[{:=2d}:{:=2d}:{:=2d}] ") << td.hours() << td.minutes() << td.seconds());

	// debug string
	m_log << "ERROR:" << string << "\n";
}
