#pragma once

#include "GameParams.h"

class Player
{
public:
	Player();
	~Player(void);

	void reset();

	void add_points(int points);
	int get_score();

private:
	int m_score;
};
