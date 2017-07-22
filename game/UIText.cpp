#include "UIText.h"

UIText::UIText(int capacity, Font* font)
	: UIItem(100, 100)
{
	m_text = new Text(capacity);

	m_text->setFont(font);
	m_text->setAnchor(Text::ANCHOR_X_LEFT | Text::ANCHOR_Y_TOP);
	m_text->setPosition(0.0f, 0.0f);
	m_text->setScale(1.0f, 1.0f);
	m_text->setKerning(0);
}

UIText::~UIText()
{
	delete m_text;
}


void UIText::setAnchor(int anchor)
{
	m_text->setAnchor(anchor);
}

void UIText::setText(const char* text)
{
	m_text->set(text);
}

void UIText::setText(int number, int num_digits, bool leading_zeros)
{
	m_text->set(number, num_digits, leading_zeros);
}

void UIText::setPosition(float x, float y)
{
	UIItem::setPosition(x, y);
}

void UIText::onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y)
{
	// don't draw if item is not enabled
	if (!m_enabled)
		return;

	glUniform4f(context->color, m_color.r, m_color.g, m_color.b, m_color.a);
	m_text->setPosition(m_x + parent_x + m_trans_pos.x, m_y + parent_y + m_trans_pos.y);
	m_text->render(context, parent_x, parent_y);
}

void UIText::onUpdate(int time)
{
}

void UIText::onRemove()
{
}

void UIText::onTouch(UI::UITouchEvent event, float x, float y)
{
}