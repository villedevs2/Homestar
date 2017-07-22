#include "UITransition.h"

UITransition::UITransition()
{
	m_pos = glm::vec2(0.0f, 0.0f);
	m_scale = glm::vec2(1.0f, 1.0f);
}

UITransition::~UITransition()
{
}

const glm::vec2& UITransition::getPosition()
{
	return m_pos;
}

const glm::vec2& UITransition::getScale()
{
	return m_scale;
}



// UITransitionLR
// ----------------------------------------------------------------------------

UITransitionLR::UITransitionLR(float start, float end, float size)
{
	m_start = start;
	m_end = end;
	m_size = size;

	m_pos = glm::vec2(0.0f, 0.0f);
	m_scale = glm::vec2(1.0f, 1.0f);
}

void UITransitionLR::update(float phase, UITransition::State state)
{
	if (state != UITransition::STATE_OFF)
	{	
		float startx;
		if (state == UITransition::STATE_IN)
			startx = -1.0f;
		else
			startx = 0.0f;

		if (phase < m_start)
		{
			m_pos = glm::vec2((0.0f + startx) * m_size, 0.0f);
		}
		else if (phase >= m_end)
		{
			m_pos = glm::vec2((1.0f + startx) * m_size, 0.0f);
		}
		else
		{
			float i = (float)(phase - m_start) / (float)(m_end - m_start);
			m_pos = glm::vec2((i + startx) * m_size, 0.0f);
		}
	}
	else
	{
		m_pos = glm::vec2(0.0f, 0.0f);
		m_scale = glm::vec2(1.0f, 1.0f);
	}
}


// UITransitionUD
// ----------------------------------------------------------------------------

UITransitionUD::UITransitionUD(float start, float end, float size)
{
	m_start = start;
	m_end = end;
	m_size = size;

	m_pos = glm::vec2(0.0f, 0.0f);
	m_scale = glm::vec2(1.0f, 1.0f);
}

void UITransitionUD::update(float phase, UITransition::State state)
{
	if (state == UITransition::STATE_IN)
	{
		if (phase < m_start)
		{
			m_pos = glm::vec2(0.0f, -1.0f * m_size);
		}
		else if (phase >= m_end)
		{
			m_pos = glm::vec2(0.0f, 0.0f);
		}
		else
		{
			float i = (float)(phase - m_start) / (float)(m_end - m_start);
			m_pos = glm::vec2(0.0f, (-1.0f + i) * m_size);
		}
	}
	else if (state == UITransition::STATE_OUT)
	{
		if (phase < m_start)
		{
			m_pos = glm::vec2(0.0f, 0.0f);
		}
		else if (phase >= m_end)
		{
			m_pos = glm::vec2(0.0f, -1.0f * m_size);
		}
		else
		{
			float i = (float)(phase - m_start) / (float)(m_end - m_start);
			m_pos = glm::vec2(0.0f, (0.0f - i) * m_size);
		}
	}
	else
	{
		m_pos = glm::vec2(0.0f, 0.0f);
		m_scale = glm::vec2(1.0f, 1.0f);
	}
}