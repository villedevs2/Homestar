#pragma once

#include "Platform.h"
#include "Debug.h"
#include "format.h"
#include <assert.h>

namespace Screen
{
	static int m_screen_width;
	static int m_screen_height;
	static float m_screen_aspect;

	int getWidth();
	int getHeight();
	float getAspect();
	float getWidthFraction(float fraction);
	float getHeightFraction(float fraction);
	float getWidthSlice(float divider);
	float getHeightSlice(float divider);
	void setup(int width, int height);
};