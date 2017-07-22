#pragma once

#include "Platform.h"
#include "Debug.h"
#include "glm/glm.hpp"

class UITransition
{
public:
	enum State
	{
		STATE_OFF,
		STATE_IN,
		STATE_OUT,
	};

	UITransition();
	~UITransition();

	virtual const glm::vec2& getPosition();
	virtual const glm::vec2& getScale();
	virtual void update(float phase, UITransition::State state) = 0;

protected:
	glm::vec2 m_pos;
	glm::vec2 m_scale;
};

class UITransitionLR : public UITransition
{
public:
	UITransitionLR(float start, float end, float size);
	~UITransitionLR();

	virtual void update(float phase, UITransition::State state);

private:
	float m_start;
	float m_end;
	float m_size;
};

class UITransitionUD : public UITransition
{
public:
	UITransitionUD(float start, float end, float size);
	~UITransitionUD();

	virtual void update(float phase, UITransition::State state);

private:
	float m_start;
	float m_end;
	float m_size;
};