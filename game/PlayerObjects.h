#define _USE_MATH_DEFINES				// this must be defined before *any* includes
#include "DynamicObject.h"

#include <cmath>

class PlayerTire : public DynamicObject
{
public:
	PlayerTire(PolyAnim* animation, Atlas* atlas);
	~PlayerTire();
};


class PlayerSoccerBall : public DynamicObject
{
public:
	PlayerSoccerBall(PolyAnim* animation, Atlas* atlas);
	~PlayerSoccerBall();
};


class PlayerBountyHunter : public DynamicObject
{
public:
	PlayerBountyHunter(PolyAnim* animation, PolyAnim* effect, Atlas* atlas);
	~PlayerBountyHunter();

	virtual void render(const Shaders::GameShaderContext* context);

private:
	PolyAnim* m_effect;
};


class PlayerReactor : public DynamicObject
{
public:
	PlayerReactor(PolyAnim* animation, Atlas* atlas);
	~PlayerReactor();
};


class PlayerHamsterWheel : public DynamicObject
{
public:
	PlayerHamsterWheel(PolyAnim* animation, PolyAnim* left, PolyAnim* right, Atlas* atlas);
	~PlayerHamsterWheel();

	virtual void render(const Shaders::GameShaderContext* context);

private:
	PolyAnim* m_left;
	PolyAnim* m_right;
};