#pragma once

#include "glm/glm.hpp"

#include "Texture2.h"
#include "Shaders.h"
#include "PolyAnim.h"

class DynamicObject
{
public:
	enum ColliderType
	{
		COLLIDER_PLAYER = 1,
		COLLIDER_NEUTRAL,
		COLLIDER_COLLECTABLE,
		COLLIDER_ENDZONE,
		COLLIDER_DESTRUCTIBLE,
	};

	DynamicObject(DynamicObject::ColliderType collider_type, PolyAnim* animation, Atlas* atlas);
	~DynamicObject();

	virtual void render(const Shaders::GameShaderContext *context);
	virtual void setAngle(float angle);
	virtual void setPosition(float x, float y);
	virtual void setPosition(const glm::vec2& position);
	virtual void setVelocity(const glm::vec2& velocity);
	virtual void setAngularVelocity(float velocity);	
	virtual void setGravityAngle(float angle);
	virtual void setZ(float z);
	virtual void update(int delta_time);
	DynamicObject::ColliderType getColliderType();
	float getX();
	float getY();
	float getZ();
	const glm::vec2& getPosition();
	const glm::vec2& getVelocity();
	float getAngularVelocity();

protected:
	PolyAnim* m_anim;
	Atlas* m_atlas;
	glm::vec2 m_position;
	float m_angle;
	ColliderType m_collider;
	float m_z;
	glm::vec2 m_velocity;
	float m_angular_velocity;
	float m_gravity_angle;

	int m_current_frame;

	timeunit m_time;
};