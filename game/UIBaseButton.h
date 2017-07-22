#pragma once

#include "Shaders.h"
#include "UI.h"
#include "UIItem.h"

class UIBaseButton : public UIItem
{
public:
	enum State
	{
		STATE_UP,
		STATE_DOWN,
	};

	UIBaseButton(int id, float x, float y, float width, float height);
	UIBaseButton(float x, float y, float width, float height);
	~UIBaseButton();

	virtual void onTouch(UI::UITouchEvent event, float x, float y);
	virtual void onRemove() = 0;
	virtual void onUpdate(int time) = 0;

protected:
	State m_state;

private:
	void init();	
};