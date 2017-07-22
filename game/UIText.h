#pragma once

#include "UI.h"
#include "UIItem.h"
#include "Font.h"
#include "Text.h"

class UIText : public UIItem
{
public:
	UIText(int capacity, Font* font);
	virtual ~UIText();

	void setText(const char* text);
	void setText(int number, int num_digits, bool leading_zeros);
	void setKerning(float kerning);
	void setPosition(float x, float y);
	void setAnchor(int anchor);
	void onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y);
	virtual void onUpdate(int time);
	virtual void onRemove();
	virtual void onTouch(UI::UITouchEvent event, float x, float y);

private:
	Text* m_text;

};