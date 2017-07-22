#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "UI.h"
#include "UIItem.h"

#include <vector>
using namespace std;

class UILayer : public UIItem
{
public:
	UILayer();
	~UILayer();

	void onUpdate(int time);
	void onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y);
	void onTouch(UI::UITouchEvent type, float x, float y);
	void onRemove();

	void addItem(const UIItem* item);
	void removeItem(int item);
	void removeAllItems();
	int getNumItems();

	void setPosition(float x, float y);

	void setEventCallback(UI::UIEventCallback* callback);

	void updateTransition(float phase, UITransition::State state);

private:
	vector<UIItem*> m_items;

	float m_x;
	float m_y;

	UI::UIEventCallback* m_event_callback;
};
