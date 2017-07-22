#pragma once

#include "glm/glm.hpp"

#include "QuadMesh2.h"

#include "Shaders.h"
#include "Font.h"

#include <assert.h>

class Text
{
public:
	static const int ANCHOR_X_LEFT = 0;
	static const int ANCHOR_X_CENTER = 1 << 0;
	static const int ANCHOR_X_RIGHT = 2 << 0;
	static const int ANCHOR_Y_TOP = 0;
	static const int ANCHOR_Y_CENTER = 1 << 2;
	static const int ANCHOR_Y_BOTTOM = 2 << 2;

	Text(int capacity);
	~Text();

	void render(const Shaders::UIShaderContext* context, float parent_x, float parent_y);
	void setFont(Font* font);
	void set(const char* text);
	void set(int number, int num_digits, bool leading_zeros);
	void setPosition(float x, float y);
	void setScale(float sx, float sy);
	void setAnchor(int anchor);
	void setKerning(float kerning);

private:
	void build(const char* text, bool full_refresh);

	QuadMesh2* m_mesh;

	int m_capacity;

	float m_x;
	float m_y;
	float m_scalex;
	float m_scaley;
	
	float m_kerning;

	char* m_text;
	int m_length;

	Font* m_font;

	float m_anchor_x;
	float m_anchor_y;

	int m_box_width;
	int m_box_height;

	int m_anchor;
};
