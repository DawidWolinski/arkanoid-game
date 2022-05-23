#pragma once
#include "paddle_animation.h"


class PaddleStandard : public PaddleAnimation
{
public:

	PaddleStandard();
	void animation(Paddle& paddle, const float& dt) override;

private:

	float animation_time;
	int paddle_width;

};

