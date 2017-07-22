#pragma once

#include "UIItem.h"

#include <vector>
using namespace std;

class UIMenu : public UIItem
{
public:
	UIMenu(float x, float y);
	virtual ~UIMenu();

	void onUpdate(int time);
	void onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y);
	void onTouch(UI::UITouchEvent type, float x, float y);

	void addItem(const UIItem* item);
	void removeItem(int item);
	void removeAllItems();
	int getNumItems();
	
	void setSpacing(float spacing);

	virtual void setEventCallback(UI::UIEventCallback* callback);
	virtual void onRemove();
	virtual void updateTransition(float phase, UITransition::State state);

private:
	void recalculateDimensions();
	float m_spacing;

	vector<UIItem*> m_items;
};