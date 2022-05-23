#pragma once
#include "boss_animation.h"

class BossOpening : public BossAnimation
{
public:
	BossOpening();
	void animation(Boss& boss, Paddle& paddle, const float& dt) override;

private:
	float animation_time;
};

