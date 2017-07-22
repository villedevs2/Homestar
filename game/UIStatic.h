#pragma once

#include "UIDrawable.h"

class UIStatic : public UIDrawable
{
public:
	UIStatic(float x, float y, float width, float height, Texture* texture);
	virtual ~UIStatic();

	virtual void onRemove();
	virtual void onUpdate(int time);
	virtual void onTouch(UI::UITouchEvent event, float x, float y);
};
