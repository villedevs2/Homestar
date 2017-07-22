#pragma once

#include "Shaders.h"

#include "UI.h"
#include "UITransition.h"

class UIItem
{
public:
	UIItem(int id, float x, float y, float width, float height);
	UIItem(float x, float y, float width, float height);
	UIItem(float width, float height);
	~UIItem();

	virtual void setPosition(float x, float y);
	virtual void setSize(float width, float height);
	virtual float getWidth();
	virtual float getHeight();
	virtual void setId(int id);
	virtual void setEventCallback(UI::UIEventCallback* callback);
	void setTransition(UITransition* transition);
	void enable();
	void disable();
	bool isEnabled();
	void setColor(float r, float g, float b, float a);
	virtual void updateTransition(float phase, UITransition::State state);

	virtual void onTouch(UI::UITouchEvent event, float x, float y) = 0;
	virtual void onUpdate(int time) = 0;
	virtual void onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y) = 0;
	virtual void onRemove() = 0;


protected:
	bool isInside(float x, float y);

	float m_x;
	float m_y;
	float m_width;
	float m_height;

	int m_id;

	UI::UIEventCallback* m_event_callback;

	UITransition* m_transition;
	glm::vec2 m_trans_pos;
	glm::vec2 m_trans_scale;
	glm::vec4 m_color;

	bool m_enabled;

private:
	void init(int id, float x, float y, float width, float height);
};
