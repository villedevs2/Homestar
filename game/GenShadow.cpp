#include "GenShadow.h"

float GenShadow::pixelLight(float x, float y)
{
	return 1.0f;
}

float GenShadow::pixelDark(float x, float y)
{
	return 0.0f;
}

float GenShadow::pixelBlob(float x, float y)
{
	float xl = 0.5f - x;
	float yl = 0.5f - y;

	float l = sqrt((xl*xl)+(yl*yl)) / 0.5f;

	l = std::min(l, 1.0f);
	l = std::max(l, 0.0f);

	return l;
}

float GenShadow::pixelJump(float x, float y)
{
	if (x < 0.5f)
	{
		return 1.0f;
	}
	else
	{
		float xd = (x - 0.5f) * 2.0f;
		float yd = 1.0f - y;
		float s = 1.0f - (xd * yd);

		s = std::min(s, 1.0f);
		s = std::max(s, 0.0f);

		return s;
	}
}

float GenShadow::pixelWall(float x, float y)
{
	if (x < 0.5f)
	{
		return 1.0f;
	}
	else
	{
		float xd = 1.0f - ((x - 0.5f) * 2.0f);

		float s = xd;

		s = std::min(s, 1.0f);
		s = std::max(s, 0.0f);

		return s;
	}
}

void GenShadow::generateFullLight(unsigned int* buffer, int sheet_width, int x, int y, int width, int height)
{
	for (int j=y; j < (y+height); j++)
	{
		for (int i=x; i < (x+width); i++)
		{
			float px = (float)(i - x) / (float)(width);
			float py = (float)(j - y) / (float)(height);
			int v = (int)(GenShadow::pixelLight(px, py) * 255);
			buffer[(j * sheet_width) + i] = (v << 16) | (v << 8) | (v << 0) | 0xff000000;
		}
	}
}

void GenShadow::generateFullDark(unsigned int* buffer, int sheet_width, int x, int y, int width, int height)
{
	for (int j=y; j < (y+height); j++)
	{
		for (int i=x; i < (x+width); i++)
		{
			float px = (float)(i - x) / (float)(width);
			float py = (float)(j - y) / (float)(height);
			int v = (int)(GenShadow::pixelDark(px, py) * 255);
			buffer[(j * sheet_width) + i] = (v << 16) | (v << 8) | (v << 0) | 0xff000000;
		}
	}
}

void GenShadow::generateBlob(unsigned int* buffer, int sheet_width, int x, int y, int width, int height)
{
	for (int j=y; j < (y+height); j++)
	{
		for (int i=x; i < (x+width); i++)
		{
			float px = (float)(i - x) / (float)(width);
			float py = (float)(j - y) / (float)(height);
			int v = (int)(GenShadow::pixelBlob(px, py) * 255);
			buffer[(j * sheet_width) + i] = (v << 16) | (v << 8) | (v << 0) | 0xff000000;
		}
	}
}

void GenShadow::generateJumpLeft(unsigned int* buffer, int sheet_width, int x, int y, int width, int height)
{
	for (int j=y; j < (y+height); j++)
	{
		for (int i=x; i < (x+width); i++)
		{
			float px = (float)(i - x) / (float)(width);
			float py = (float)(j - y) / (float)(height);
			int v = (int)(GenShadow::pixelJump(px, py) * 255);
			buffer[(j * sheet_width) + i] = (v << 16) | (v << 8) | (v << 0) | 0xff000000;
		}
	}
}

void GenShadow::generateJumpRight(unsigned int* buffer, int sheet_width, int x, int y, int width, int height)
{
	for (int j=y; j < (y+height); j++)
	{
		for (int i=x; i < (x+width); i++)
		{
			float px = (float)(x+width-i) / (float)(width);
			float py = (float)(j - y) / (float)(height);
			int v = (int)(GenShadow::pixelJump(px, py) * 255);
			buffer[(j * sheet_width) + i] = (v << 16) | (v << 8) | (v << 0) | 0xff000000;
		}
	}
}

void GenShadow::generateJumpCombo(unsigned int* buffer, int sheet_width, int x, int y, int width, int height)
{
	for (int j=y; j < (y+height); j++)
	{
		for (int i=x; i < (x+width); i++)
		{
			float px1 = (float)(i - x) / (float)(width);
			float py1 = (float)(j - y) / (float)(height);

			float px2 = (float)(x+width-i) / (float)(width);
			float py2 = (float)(j - y) / (float)(height);

			int mix = (int)((GenShadow::pixelJump(px1, py1) * GenShadow::pixelJump(px2, py2)) * 255);

			buffer[(j * sheet_width) + i] = (mix << 16) | (mix << 8) | (mix << 0) | 0xff000000;
		}
	}
}

void GenShadow::generateWallLeft(unsigned int* buffer, int sheet_width, int x, int y, int width, int height)
{
	for (int j=y; j < (y+height); j++)
	{
		for (int i=x; i < (x+width); i++)
		{
			float px = (float)(i - x) / (float)(width);
			float py = (float)(j - y) / (float)(height);
			int v = (int)(GenShadow::pixelWall(px, py) * 255);
			buffer[(j * sheet_width) + i] = (v << 16) | (v << 8) | (v << 0) | 0xff000000;
		}
	}
}

void GenShadow::generateWallRight(unsigned int* buffer, int sheet_width, int x, int y, int width, int height)
{
	for (int j=y; j < (y+height); j++)
	{
		for (int i=x; i < (x+width); i++)
		{
			float px = (float)(x + width - i) / (float)(width);
			float py = (float)(j - y) / (float)(height);
			int v = (int)(GenShadow::pixelWall(px, py) * 255);
			buffer[(j * sheet_width) + i] = (v << 16) | (v << 8) | (v << 0) | 0xff000000;
		}
	}
}

void GenShadow::generateWallCombo(unsigned int* buffer, int sheet_width, int x, int y, int width, int height)
{
	for (int j=y; j < (y+height);j ++)
	{
		for (int i=x; i < (x+width); i++)
		{
			float px1 = (float)(i - x) / (float)(width);
			float py1 = (float)(j - y) / (float)(height);

			float px2 = (float)(x+width-i) / (float)(width);
			float py2 = (float)(j - y) / (float)(height);

			int mix = (int)((GenShadow::pixelWall(px1, py1) * GenShadow::pixelWall(px2, py2)) * 255);

			buffer[(j * sheet_width) + i] = (mix << 16) | (mix << 8) | (mix << 0) | 0xff000000;
		}
	}
}

// wall left, jump right
void GenShadow::generateWLJR(unsigned int* buffer, int sheet_width, int x, int y, int width, int height)
{
	for (int j=y; j < (y+height); j++)
	{
		for (int i=x; i < (x+width); i++)
		{
			float px1 = (float)(i - x) / (float)(width);
			float py1 = (float)(j - y) / (float)(height);

			float px2 = (float)(x+width-i) / (float)(width);
			float py2 = (float)(j - y) / (float)(height);
			
			int v = (int)((GenShadow::pixelWall(px1, py1) * GenShadow::pixelJump(px2, py2)) * 255);

			buffer[(j * sheet_width) + i] = (v << 16) | (v << 8) | (v << 0) | 0xff000000;
		}
	}
}

void GenShadow::generateJLWR(unsigned int* buffer, int sheet_width, int x, int y, int width, int height)
{
	for (int j=y; j < (y+height); j++)
	{
		for (int i=x; i < (x+width); i++)
		{

			float px1 = (float)(i - x) / (float)(width);
			float py1 = (float)(j - y) / (float)(height);

			float px2 = (float)(x + width - i) / (float)(width);
			float py2 = (float)(j - y) / (float)(height);

			int v = (int)((GenShadow::pixelJump(px1, py1) * GenShadow::pixelWall(px2, py2)) * 255);
			
			buffer[(j * sheet_width) + i] = (v << 16) | (v << 8) | (v << 0) | 0xff000000;
		}
	}
}

void GenShadow::generateVignette(unsigned int* buffer, int sheet_width, int x, int y, int width, int height)
{
	for (int j=y; j < (y+height); j++)
	{
		for (int i=x; i < (x+width); i++)
		{
			float px = (float)(i - x) / (float)(width);
			float py = (float)(j - y) / (float)(height);
			int v = (int)(((GenShadow::pixelBlob(px, py) * 0.9f) - 0.0f) * 255);
			buffer[(j * sheet_width) + i] = (v << 24) | 0x000000;
		}
	}
}
