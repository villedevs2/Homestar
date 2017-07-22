#include "Screen.h"

int Screen::getWidth()
{
	return m_screen_width;
}

int Screen::getHeight()
{
	return m_screen_height;
}

float Screen::getAspect()
{
	return m_screen_aspect;
}

float Screen::getWidthFraction(float fraction)
{
	assert(fraction >= 0.0f && fraction <= 1.0f);
	return (float)(m_screen_width) * fraction;
}

float Screen::getHeightFraction(float fraction)
{
	assert(fraction >= 0.0f && fraction <= 1.0f);
	return (float)(m_screen_height) * fraction;
}

float Screen::getWidthSlice(float divider)
{
	if (divider == 0.0f)
		return 0.0f;
	return (float)(m_screen_width) / divider;
}

float Screen::getHeightSlice(float divider)
{
	if (divider == 0.0f)
		return 0.0f;
	return (float)(m_screen_height) / divider;
}

void Screen::setup(int width, int height)
{
	m_screen_width = width;
	m_screen_height = height;
	m_screen_aspect = (float)(width) / (float)(height);

	Debug::log(str(fmt::Format("Screen Width: {:d}, Height: {:d}, Aspect: {:f}") << m_screen_width << m_screen_height << m_screen_aspect));
}