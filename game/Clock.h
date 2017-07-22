#pragma once

#include "Platform.h"
#include <format/format.h>

class Clock
{
public:
	Clock();
	~Clock();

	void reset();
	void add(int delta);
	bool sub(int delta);
	void set(int minutes, int seconds, int milliseconds);
	timeunit get();
	const std::string& format();

private:
	timeunit m_count;
	std::string m_text;
};