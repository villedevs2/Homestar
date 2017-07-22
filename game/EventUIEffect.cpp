#include "EventUIEffect.h"

EventUIEffect::EventUIEffect(int num, Text** text)
{
	m_effect = new Effect[num];

	for (int i=0; i < num; i++)
	{
		m_effect[i].dir = glm::vec2(0,0);
		m_effect[i].duration = 0;
		m_effect[i].timer = 0;
		m_effect[i].pos = glm::vec2(0,0);
		m_effect[i].event = 0;
	}

	m_text = text;

	m_max = num;
	m_current = 0;
}

EventUIEffect::~EventUIEffect()
{
	delete [] m_effect;
}

void EventUIEffect::addEvent(int event, int duration)
{
	m_current++;
	m_current %= m_max;

	float length = 120.0f / 60.0f;

	m_effect[m_current].timer = duration * 1000000;
	m_effect[m_current].duration = duration * 1000000;
	m_effect[m_current].pos = glm::vec2(400, 240);		// TOODOOOOO
	m_effect[m_current].dir = glm::vec2(0, -length);		// initially no movement
	m_effect[m_current].event = event;
}

void EventUIEffect::update(int time)
{
	for (int i=0; i < m_max; i++)
	{
		if (m_effect[i].timer > 0)
		{
			m_effect[i].timer -= time;
			m_effect[i].pos += m_effect[i].dir;
		}
	}
}

void EventUIEffect::render(const Shaders::UIShaderContext* context)
{
	for (int i=0; i < m_max; i++)
	{
		if (m_effect[i].timer > 0)
		{
			int event = m_effect[i].event;

			int ii = (m_current - i) % m_max;

			int elapsed_time = m_effect[i].duration - m_effect[i].timer;

			float s = 0.6f;
			if (elapsed_time < 500000000)
			{
				s += ((float)(500000000 - elapsed_time) / 500000000.0f) * 4.5f;
			}

			m_text[event]->setScale(s, s);
			//m_text->setPosition(m_effect[i].pos.x(), m_effect[i].pos.y());
			m_text[event]->setPosition(400, (float)(240 - (ii * 30)));
			m_text[event]->render(context, 0.0f, 0.0f);
		}
	}
}
