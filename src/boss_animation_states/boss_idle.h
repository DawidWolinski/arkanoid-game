#pragma once
#include "boss_animation.h"

class BossIdle : public BossAnimation
{
public:
	BossIdle();
	void animation(Boss& boss, Paddle& paddle, const float& dt) override;

private:
	float animation_time;
};