#include "Player.h"

Player::Player()
{
	reset();
}

Player::~Player(void)
{
}

void Player::reset()
{
	m_score = 0;
}


void Player::add_points(int points)
{
	m_score += points;
}

int Player::get_score()
{
	return m_score;
}
