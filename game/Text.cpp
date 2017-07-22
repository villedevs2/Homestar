#include "Text.h"

Text::Text(int capacity)
{
	m_capacity = capacity;

	m_mesh = new QuadMesh2(capacity);

	m_text = new char[capacity];
	memset(m_text, 0, capacity);
	m_length = 0;

	m_x = 0;
	m_y = 0;
	m_scalex = 1.0f;
	m_scaley = 1.0f;

	m_kerning = 0.0f;

	m_box_width = 0;
	m_box_height = 0;

	m_anchor_x = 0;
	m_anchor_y = 0;

	m_anchor = ANCHOR_X_LEFT | ANCHOR_Y_TOP;

	m_font = NULL;
}

Text::~Text(void)
{
	if (m_mesh)
		delete m_mesh;
	if (m_text)
		delete [] m_text;
}

void Text::setFont(Font* font)
{
	m_font = font;
}

void Text::setPosition(float x, float y)
{
	m_x = x;
	m_y = y;
}

void Text::setScale(float sx, float sy)
{
	m_scalex = sx;
	m_scaley = sy;
}

void Text::setKerning(float kerning)
{
	bool refresh = kerning != m_kerning;

	m_kerning = kerning;

	if (refresh)
		build(m_text, true);
}

void Text::set(const char* text)
{
	build(text, false);
}

void Text::build(const char* text, bool full_refresh)
{
	if (m_font == 0)		// in case font not loaded yet
		return;

	m_length = strlen(text);
	if (m_length >= m_capacity)
		m_length = m_capacity - 1;

	if (m_length <= 0)
		return;


	float x = 0.0f;
	float y = 0.0f;

	float hmargin = (float)m_font->getHMargin();

	int max_width = 0;
	int mesh_index = 0;

	float prev_underspace = 0;

	for (int i=0; i < m_length; i++)
	{
		int c = text[i] & 0x7f;					// mask to the first 128 ascii chars
		int old_c = m_text[i];

	//	if (c != old_c || full_refresh)			// update if the new character is different
		if (1)
		{
			m_text[i] = c;

			if (c == 0x20)						// insert empty space for space char
			{
				x += m_font->getGlyph(0x30)->width + hmargin + m_kerning;		// width of '0'
				prev_underspace = 0;
			}
			else if (c == 0x0a)					// newline
			{
				if (x > max_width)
					max_width = (int)x;

				x = 0.0f;
				y += m_font->getLineHeight();
				prev_underspace = 0;
			}
			else								// normal characters
			{
				const Font::Glyph* glyph = m_font->getGlyph(c);

				float tu1 = glyph->u1;
				float tu2 = glyph->u2;
				float tv1 = glyph->v1;
				float tv2 = glyph->v2;

				float width = (float)glyph->width;
				float height = (float)glyph->height;

				if (glyph->use_underspace)
					x -= prev_underspace;

				m_mesh->set(mesh_index, glm::vec2(x, y), glm::vec2(x + width, y), glm::vec2(x, y + height), glm::vec2(x + width, y + height),
							glm::vec2(tu1, tv1), glm::vec2(tu2, tv1), glm::vec2(tu1, tv2), glm::vec2(tu2, tv2));

				mesh_index++;

				x += glyph->width + hmargin + m_kerning;

				prev_underspace = glyph->underspace;
			}
		}
		else	// old character, just skip over it
		{
			const Font::Glyph* glyph = m_font->getGlyph(c);
			x += glyph->width + hmargin + m_kerning;
		}
	}

	m_mesh->set_num(mesh_index);

	if (x > max_width)
		max_width = (int)x;

	m_box_width = max_width;
	m_box_height = (int)(y + m_font->getLineHeight());


	int anchor_x_flag = m_anchor & 0x3;
	int anchor_y_flag = m_anchor & 0xc;

	if (anchor_x_flag == ANCHOR_X_CENTER)
		m_anchor_x = (float)(m_box_width / 2);
	else if (anchor_x_flag == ANCHOR_X_RIGHT)
		m_anchor_x = (float)(m_box_width);
	else
		m_anchor_x = 0;

	if (anchor_y_flag == ANCHOR_Y_CENTER)
		m_anchor_y = (float)(m_box_height / 2);
	else if (anchor_y_flag == ANCHOR_Y_BOTTOM)
		m_anchor_y = (float)(m_box_height);
	else
		m_anchor_y = 0;
}

void Text::set(int number, int num_digits, bool leading_zeros)
{
	static char buffer[32];

	for (int i=0; i < num_digits; i++)
	{
		int digit = number % 10;

		if (digit != 0 || leading_zeros)
		{
			buffer[num_digits-i-1] = (char)(0x30 + digit);
		}
		else
		{
			buffer[num_digits-i-1] = 0x20;	// space
		}

		number /= 10;
	}

	buffer[num_digits] = 0;

	set(buffer);
}

void Text::setAnchor(int anchor)
{
	if (m_anchor != anchor)
	{
		m_anchor = anchor;

		build(m_text, true);
	}
}

void Text::render(const Shaders::UIShaderContext* context, float parent_x, float parent_y)
{
	if (m_font == 0)
		return;

	m_font->apply(0);

	glUniform2f(context->location, m_x - (m_anchor_x * m_scalex) + parent_x, m_y - (m_anchor_y * m_scaley) + parent_y);
	glUniform2f(context->scale, m_scalex, m_scaley);

	m_mesh->render(context);
}