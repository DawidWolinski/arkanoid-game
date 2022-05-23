#pragma once
#include "../engine.h"


class Paddle;

class PaddleAnimation
{
public:
	virtual void animation(Paddle& paddle, const float& dt) = 0;
};

