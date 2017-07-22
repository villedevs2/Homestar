#include "UIBaseButton.h"

UIBaseButton::UIBaseButton(int id, float x, float y, float width, float height)
	: UIItem(id, x, y, width, height)
{
	init();
}

UIBaseButton::UIBaseButton(float x, float y, float width, float height)
	: UIItem(x, y, width, height)
{
	init();
}

UIBaseButton::~UIBaseButton()
{
}

void UIBaseButton::init()
{
	m_state = STATE_UP;
}

void UIBaseButton::onTouch(UI::UITouchEvent event, float x, float y)
{
	// don't handle if this item is not enabled
	if (!m_enabled)
		return;

	bool inside = isInside(x, y);

	if (event == UI::UI_TOUCH_DOWN)
	{
		if (inside)
		{
			m_state = STATE_DOWN;

			// TODO: callback for button down?
		}
	}
	else if (event == UI::UI_TOUCH_UP)
	{
		if (inside)
		{
			// throw a click event if button was pushed down
			if (m_state == STATE_DOWN)
			{
				if (m_event_callback != NULL)
				{
					m_event_callback->onUIEvent(m_id, UI::UI_BUTTON_CLICK);
				}
			}
		}

		// TODO: UI_BUTTON_UP event!
		
		// touch anywhere on screen pulls the button up
		m_state = STATE_UP;
	}
}