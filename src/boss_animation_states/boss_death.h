#pragma once
#include "boss_animation.h"

class BossDeath : public BossAnimation
{
public:

	BossDeath();
	void animation(Boss& boss, Paddle& paddle, const float& dt) override;

private:

	float animation_time;
	int i;

};

