#include "UIItem.h"

UIItem::UIItem(int id, float x, float y, float width, float height)
{
	init(id, x, y, width, height);
}

UIItem::UIItem(float x, float y, float width, float height)
{
	init(-1, x, y, width, height);
}

UIItem::UIItem(float width, float height)
{
	init(-1, 0.0f, 0.0f, width, height);
}

UIItem::~UIItem()
{
}

void UIItem::init(int id, float x, float y, float width, float height)
{
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
	m_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_event_callback = NULL;

	m_transition = NULL;
	m_trans_pos = glm::vec2(0.0f, 0.0f);
	m_trans_scale = glm::vec2(1.0f, 1.0f);

	m_id = id;

	m_enabled = true;
}

bool UIItem::isInside(float x, float y)
{
	// TODO: extents based on anchor point
	float x1 = m_x - (m_width / 2);
	float x2 = x1 + m_width;
	float y1 = m_y - (m_height / 2);
	float y2 = y1 + m_height;

	if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
		return true;
	else
		return false;
}

void UIItem::setPosition(float x, float y)
{
	m_x = x;
	m_y = y;
}

void UIItem::setSize(float width, float height)
{
	m_width = width;
	m_height = height;
}

float UIItem::getWidth() { return m_width; }
float UIItem::getHeight() { return m_height; }

void UIItem::setId(int id)
{
	m_id = id;
}

void UIItem::setEventCallback(UI::UIEventCallback* callback)
{
	m_event_callback = callback;
}

void UIItem::enable()
{
	m_enabled = true;
}

void UIItem::disable()
{
	m_enabled = false;
}

bool UIItem::isEnabled()
{
	return m_enabled;
}

void UIItem::setColor(float r, float g, float b, float a)
{
	m_color = glm::vec4(r, g, b, a);
}

void UIItem::setTransition(UITransition* transition)
{
	m_transition = transition;
}

void UIItem::updateTransition(float phase, UITransition::State state)
{
	if (m_transition != NULL)
	{
		m_transition->update(phase, state);

		m_trans_pos = m_transition->getPosition();
		m_trans_scale = m_transition->getScale();
	}
}