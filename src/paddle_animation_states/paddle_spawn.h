#pragma once
#include "paddle_animation.h"


class PaddleSpawn : public PaddleAnimation
{
public:

	void animation(Paddle& paddle, const float& dt) override;

private:

	float animation_time;

};

