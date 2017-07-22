#include "Clock.h"

Clock::Clock()
{
	m_count = 0;
}

Clock::~Clock()
{

}

void Clock::reset()
{
	m_count = 0;
}

void Clock::add(int delta)
{
	m_count += delta;
}

bool Clock::sub(int delta)
{
	bool ret = false;
	m_count -= delta;

	if (m_count < 0)
	{
		ret = true;
		m_count = 0;
	}

	return ret;
}

void Clock::set(int minutes, int seconds, int milliseconds)
{
	m_count = (((timeunit)(minutes) * 60000) + ((timeunit)(seconds) * 1000) + (timeunit)(milliseconds)) * 1000000;
}

timeunit Clock::get()
{
	return m_count / 1000000;
}

const std::string& Clock::format()
{
	static char buffer[64];

	int count = m_count / 1000000;

	int mins = count / 60000;
	int secs = (count / 1000) % 60;
	int millisecs = count % 1000;
	
	m_text = str(fmt::Format("{:=d}:{:=02d}.{:=03d}") << mins << secs << millisecs);

	return m_text;
}