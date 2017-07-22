#pragma once

#include "Texture2.h"
#include "BinaryFile.h"

#include "rapidxml.hpp"

#include <string>

class Font
{
public:
	typedef struct
	{
		float x;
		float y;
		float width;
		float height;
		float u1;
		float v1;
		float u2;
		float v2;
		bool use_underspace;
		float underspace;
	} Glyph;

	Font();
	~Font();

	bool load(std::string filename, int size);
	const Font::Glyph* getGlyph(int code);
	int getLineHeight();
	int getHMargin();
	void apply(int unit);

private:
	Texture* m_texture;
	Glyph m_glyph[256];

	int m_lineheight;
	int m_hmargin;

	int m_size;
};