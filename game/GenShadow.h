#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

namespace GenShadow
{
	float pixelLight(float x, float y);
	float pixelDark(float x, float y);
	float pixelBlob(float x, float y);
	float pixelJump(float x, float y);
	float pixelWall(float x, float y);

	void generateFullLight(unsigned int* buffer, int sheet_width, int x, int y, int width, int height);
	void generateFullDark(unsigned int* buffer, int sheet_width, int x, int y, int width, int height);
	void generateBlob(unsigned int* buffer, int sheet_width, int x, int y, int width, int height);
	void generateJumpLeft(unsigned int* buffer, int sheet_width, int x, int y, int width, int height);
	void generateJumpRight(unsigned int* buffer, int sheet_width, int x, int y, int width, int height);
	void generateJumpCombo(unsigned int* buffer, int sheet_width, int x, int y, int width, int height);
	void generateWallLeft(unsigned int* buffer, int sheet_width, int x, int y, int width, int height);
	void generateWallRight(unsigned int* buffer, int sheet_width, int x, int y, int width, int height);
	void generateWallCombo(unsigned int* buffer, int sheet_width, int x, int y, int width, int height);
	void generateWLJR(unsigned int* buffer, int sheet_width, int x, int y, int width, int height);
	void generateJLWR(unsigned int* buffer, int sheet_width, int x, int y, int width, int height);
	void generateVignette(unsigned int* buffer, int sheet_width, int x, int y, int width, int height);
};
