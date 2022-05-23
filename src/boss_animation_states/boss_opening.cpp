#include "boss_opening.h"
#include "../objects/boss.h"
#include "../objects/paddle.h"


BossOpening::BossOpening()
	: animation_time(0.f)
{

}

void BossOpening::animation(Boss& boss, Paddle& paddle, const float& dt)
{
	animation_time += 10.f * dt;

	if (!paddle.is_moving)
		boss.state = &boss.closing;

	else if (animation_time > 1.f)
	{
		animation_time = 0.f;

		boss.setTextureRect(sf::IntRect(boss.getTextureRect().left + 64, boss.getTextureRect().top, 62, 96));

		if (boss.getTextureRect().left == 192) 
			boss.state = &boss.shooting; 
	}
	
}