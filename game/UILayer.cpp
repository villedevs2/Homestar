#include "UILayer.h"

UILayer::UILayer() : UIItem(1000, 1000)
{
	m_x = 0;
	m_y = 0;
}

UILayer::~UILayer()
{
}


void UILayer::addItem(const UIItem* item)
{
	m_items.push_back((UIItem*)item);
	((UIItem*)item)->setEventCallback(m_event_callback);
}

void UILayer::removeItem(int item)
{
	// TODO
}

int UILayer::getNumItems()
{
	return (int)(m_items.size());
}

void UILayer::setPosition(float x, float y)
{
	m_x = x;
	m_y = y;
}

void UILayer::setEventCallback(UI::UIEventCallback* callback)
{
	m_event_callback = callback;

	int num_items = (int)m_items.size();
	for (int i=0; i < num_items; i++)
	{
		m_items[i]->setEventCallback(m_event_callback);
	}
}

void UILayer::updateTransition(float phase, UITransition::State state)
{
	int num_items = (int)m_items.size();
	for (int i=0; i < num_items; i++)
	{
		m_items[i]->updateTransition(phase, state);
	}
}

void UILayer::onUpdate(int time)
{
	if (!m_enabled)
		return;

	int num_items = (int)m_items.size();
	for (int i=0; i < num_items; i++)
	{
		m_items[i]->onUpdate(time);
	}
}

void UILayer::onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y)
{
	if (!m_enabled)
		return;

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(context->shader);

	int num_items = (int)m_items.size();
	for (int i=0; i < num_items; i++)
	{
		m_items[i]->onRender(context, m_x + parent_x, m_y + parent_y);
	}
}

void UILayer::onTouch(UI::UITouchEvent type, float x, float y)
{
	if (!m_enabled)
		return;

	int num_items = (int)m_items.size();
	for (int i=0; i < num_items; i++)
	{
		m_items[i]->onTouch(type, x - m_x, y - m_y);
	}
}

void UILayer::onRemove()
{
}