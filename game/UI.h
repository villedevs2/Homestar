#pragma once

namespace UI
{
	enum UITouchEvent
	{
		UI_TOUCH_DOWN,
		UI_TOUCH_UP,
		UI_TOUCH_MOVE,
	};

	enum UIItemEvent
	{
		UI_BUTTON_DOWN			= 1,
		UI_BUTTON_UP,
		UI_BUTTON_CLICK,
	};

	class UIEventCallback
	{
	public:
		virtual void onUIEvent(int itemid, UI::UIItemEvent eventid) = 0;
	};
};