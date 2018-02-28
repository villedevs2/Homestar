#include "PlayerObjects.h"


PlayerTire::PlayerTire(PolyAnim* animation, Atlas* atlas) : DynamicObject(DynamicObject::COLLIDER_PLAYER, animation, atlas)
{
}

PlayerTire::~PlayerTire()
{
}



PlayerSoccerBall::PlayerSoccerBall(PolyAnim* animation, Atlas* atlas) : DynamicObject(DynamicObject::COLLIDER_PLAYER, animation, atlas)
{
}

PlayerSoccerBall::~PlayerSoccerBall()
{
}



PlayerBountyHunter::PlayerBountyHunter(PolyAnim* animation, PolyAnim* effect, Atlas* atlas) : DynamicObject(DynamicObject::COLLIDER_PLAYER, animation, atlas)
{
	m_effect = effect;
}

PlayerBountyHunter::~PlayerBountyHunter()
{
}

void PlayerBountyHunter::render(const Shaders::GameShaderContext* context)
{
	int effect_frame = ((m_time / 1000000) / 100) % m_effect->getNumFrames();

	m_atlas->apply(0);

	m_anim->render(context, m_current_frame);
	m_effect->render(context, effect_frame);
}



PlayerReactor::PlayerReactor(PolyAnim* animation, Atlas* atlas) : DynamicObject(DynamicObject::COLLIDER_PLAYER, animation, atlas)
{
}

PlayerReactor::~PlayerReactor()
{
}



PlayerShip::PlayerShip(PolyAnim* animation, Atlas* atlas) : DynamicObject(DynamicObject::COLLIDER_PLAYER, animation, atlas)
{
}

PlayerShip::~PlayerShip()
{
}

void PlayerShip::render(const Shaders::GameShaderContext* context)
{
	m_atlas->apply(0);

	int frame = ((m_time / 1000000) / 20) % m_anim->getNumFrames();
	m_anim->render(context, frame);
}




PlayerHamsterWheel::PlayerHamsterWheel(PolyAnim* animation, PolyAnim* left, PolyAnim* right, Atlas* atlas) : DynamicObject(DynamicObject::COLLIDER_PLAYER, animation, atlas)
{
	m_left = left;
	m_right = right;
}

PlayerHamsterWheel::~PlayerHamsterWheel()
{
}

void PlayerHamsterWheel::render(const Shaders::GameShaderContext* context)
{
	m_atlas->apply(0);

	m_anim->render(context, m_current_frame);

	float rot_mat[4];
	float ca = cos(m_gravity_angle * M_PI / 180.0f);
	float sa = sin(m_gravity_angle * M_PI / 180.0f);
	rot_mat[0] = ca;
	rot_mat[1] = sa;
	rot_mat[2] = -sa;
	rot_mat[3] = ca;

	glUniformMatrix2fv(context->rot_matrix, 1, false, rot_mat);

	if (abs(m_angular_velocity) > 5)
	{
		if (m_angular_velocity < 0)
		{
			int frame = ((m_time / 1000000) / 50) % m_left->getNumFrames();
			m_left->render(context, frame);
		}
		else
		{
			int frame = frame = ((m_time / 1000000) / 50) % m_right->getNumFrames();
			m_right->render(context, frame);
		}
	}
	else
	{
		m_left->render(context, 0);
	}
	
	rot_mat[0] = 1.0f;
	rot_mat[1] = 0.0f;
	rot_mat[2] = 0.0f;
	rot_mat[3] = 1.0f;

	glUniformMatrix2fv(context->rot_matrix, 1, false, rot_mat);
}