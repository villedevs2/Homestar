#pragma once

#include "glm/glm.hpp"

#include "Shaders.h"
#include "Text.h"
#include "Font.h"

class EventUIEffect
{
public:
	EventUIEffect(int num, Text** text);
	~EventUIEffect();

	void addEvent(int event, int duration);
	void update(int time);
	void render(const Shaders::UIShaderContext* context);

private:
	typedef struct
	{
		glm::vec2 pos;
		glm::vec2 dir;
		timeunit timer;
		timeunit duration;
		int event;
	} Effect;

	int m_max;
	int m_current;

	Text** m_text;

	Effect* m_effect;
};