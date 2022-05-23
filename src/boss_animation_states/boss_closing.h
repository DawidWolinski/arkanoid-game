#pragma once
#include "boss_animation.h"


class BossClosing : public BossAnimation
{
public:
	BossClosing();
	void animation(Boss& boss, Paddle& paddle, const float& dt) override;

private:
	float animation_time;
};
