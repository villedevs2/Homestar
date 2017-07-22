#include "DynamicObject.h"

DynamicObject::DynamicObject(DynamicObject::ColliderType collider_type, PolyAnim* animation, Atlas* atlas)
{
	m_collider = collider_type;
	m_anim = animation;
	m_atlas = atlas;

	m_position = glm::vec2(0,0);
	m_angle = 0;
	m_current_frame = 0;
	m_time = 0;

	m_z = 0;
}

DynamicObject::~DynamicObject()
{
}

void DynamicObject::setPosition(float x, float y)
{
	m_position = glm::vec2(x, y);
}

void DynamicObject::setPosition(const glm::vec2& position)
{
	m_position = position;
}

void DynamicObject::setAngle(float angle)
{
	m_angle = angle;
}

DynamicObject::ColliderType DynamicObject::getColliderType()
{
	return m_collider;
}

void DynamicObject::setZ(float z)
{
	m_z = z;
}

float DynamicObject::getX()
{
	return m_position.x;
}

float DynamicObject::getY()
{
	return m_position.y;
}

const glm::vec2& DynamicObject::getPosition()
{
	return m_position;
}

float DynamicObject::getZ()
{
	return m_z;
}

void DynamicObject::setVelocity(const glm::vec2& velocity)
{
	m_velocity = velocity;
}

void DynamicObject::setAngularVelocity(float velocity)
{
	m_angular_velocity = velocity;
}

void DynamicObject::setGravityAngle(float angle)
{
	m_gravity_angle = angle;
}

void DynamicObject::update(int delta_time)
{
	m_time += delta_time;

	float angle = m_angle / 10.0f;

	if (angle < 0)
	{
		m_current_frame = (int)(abs(angle)) % m_anim->getNumFrames();		
	}
	else
	{
		m_current_frame = (m_anim->getNumFrames() - 1) - ((int)(abs(angle)) % m_anim->getNumFrames());
	}
}

void DynamicObject::render(const Shaders::GameShaderContext* context)
{
	m_atlas->apply(0);

	m_anim->render(context, m_current_frame);
}