#include "UIMenu.h"

UIMenu::UIMenu(float x, float y) : UIItem(x, y, 0, 0)
{
	m_x = x;
	m_y = y;
	m_width = 0;
	m_height = 0;
	m_spacing = 0.0f;
}

UIMenu::~UIMenu()
{
}

void UIMenu::addItem(const UIItem* item)
{
	m_items.push_back((UIItem*)item);
	((UIItem*)item)->setEventCallback(m_event_callback);

	recalculateDimensions();
}

void UIMenu::removeItem(int item)
{
	m_items.erase(m_items.begin()+item, m_items.begin()+item+1);

	recalculateDimensions();
}

int UIMenu::getNumItems()
{
	return (int)(m_items.size());
}

void UIMenu::setSpacing(float spacing)
{
	m_spacing = spacing;

	recalculateDimensions();
}

void UIMenu::recalculateDimensions()
{
	float x = m_x;
	float y = m_y;
	float h = 0.0f;
	float w = 0.0f;

	int num_items = (int)(m_items.size());

	for (int i=0; i < num_items; i++)
	{
		float iwidth = m_items[i]->getWidth();

		if (iwidth > w)
			w = iwidth;
	}

	for (int i=0; i < num_items; i++)
	{
		float iwidth = m_items[i]->getWidth();
		float iheight = m_items[i]->getHeight() + m_spacing;

		// TODO: margins and stuff
		// TODO: item horiz positioning?

		float xmid = x + (w / 2);

		//  set item position
		m_items[i]->setPosition(xmid, y + (iheight / 2));

		h += iheight;
		y += iheight;
	}

	// set size of the menu item
	setSize(w, h);
}

void UIMenu::setEventCallback(UI::UIEventCallback* callback)
{
	m_event_callback = callback;

	int num_items = (int)m_items.size();
	for (int i=0; i < num_items; i++)
	{
		m_items[i]->setEventCallback(m_event_callback);
	}
}

void UIMenu::updateTransition(float phase, UITransition::State state)
{
	int num_items = (int)m_items.size();
	for (int i=0; i < num_items; i++)
	{
		m_items[i]->updateTransition(phase, state);
	}
}

void UIMenu::onUpdate(int time)
{
}

void UIMenu::onRemove()
{
}

void UIMenu::onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y)
{
	// don't draw if not enabled
	if (!m_enabled)
		return;

	int num_items = (int)m_items.size();
	for (int i=0; i < num_items; i++)
	{
		m_items[i]->onRender(context, parent_x, parent_y);
	}
}

void UIMenu::onTouch(UI::UITouchEvent type, float x, float y)
{
	// don't pass events if not enabled
	if (!m_enabled)
		return;

	int num_items = (int)m_items.size();
	for (int i=0; i < num_items; i++)
	{
		m_items[i]->onTouch(type, x, y);
	}
}