#include "boss_idle.h"
#include "../objects/boss.h"
#include "../objects/paddle.h"

BossIdle::BossIdle()
	: animation_time(0.f)
{

}

void BossIdle::animation(Boss& boss, Paddle& paddle, const float& dt)
{
	animation_time += 10.f * dt;
		
	if (animation_time > 30.f && paddle.is_moving)
	{
		animation_time = 0.f;

		boss.state = &boss.opening;
	}


}
