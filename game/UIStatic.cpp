#include "UIStatic.h"

UIStatic::UIStatic(float x, float y, float width, float height, Texture* texture)
	: UIDrawable(x, y, width, height, texture)
{
}

UIStatic::~UIStatic(void)
{
}

void UIStatic::onRemove()
{
}

void UIStatic::onUpdate(int time)
{
}

void UIStatic::onTouch(UI::UITouchEvent event, float x, float y)
{
}