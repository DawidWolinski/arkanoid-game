#include "boss_closing.h"
#include "../objects/boss.h"
#include "../objects/paddle.h"

BossClosing::BossClosing()
	: animation_time(0.f)
{

}

void BossClosing::animation(Boss& boss, Paddle& paddle, const float& dt)
{
	animation_time += 10.f * dt;

	if (animation_time > 1.f)
	{
		animation_time = 0.f;

		if (boss.getTextureRect().left == 0)
		{
			boss.state = &boss.idle;
			boss.shooting.reset();
		}
			
		else
			boss.setTextureRect(sf::IntRect(boss.getTextureRect().left - 64, boss.getTextureRect().top, 62, 96));

		
	}
}
