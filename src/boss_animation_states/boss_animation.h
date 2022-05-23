#pragma once
#include "../engine.h"

class Boss;
class Paddle;

class BossAnimation
{
public:

	virtual void animation(Boss& boss, Paddle& paddle, const float& dt) = 0;
};
