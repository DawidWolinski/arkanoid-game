#include "boss_death.h"
#include <iostream>
#include "../objects/boss.h"
#include "../objects/paddle.h"

BossDeath::BossDeath()
	: animation_time(0.f), i(0)
{

}

void BossDeath::animation(Boss& boss, Paddle& paddle, const float& dt)
{
	if (boss.is_drawing)
	{
		animation_time += 10.f * dt;

		if (i == 0)
		{
			if (boss.getTextureRect().left > 0)
				i = 1;
			else
				i = 2;
		}

		else if (i == 1 && animation_time > 1.f)
		{
			animation_time = 0.f;

			if (boss.getTextureRect().left == 0)
				i++;
			else
				boss.setTextureRect(sf::IntRect(boss.getTextureRect().left - 64, boss.getTextureRect().top, 62, 96));		
		}

		else if (i == 2 && animation_time > 15.f)
		{
			animation_time = 0.f;
			boss.setTextureRect(sf::IntRect(0, 624, 62, 96));
			i++;
		}

		else if (i == 3 && animation_time > 1.5f)
		{
			animation_time = 0.f;

			if (boss.getTextureRect().left == 0 && boss.getTextureRect().top == 816)
				i++;

			if (boss.getTextureRect().left == 192)
				boss.setTextureRect(sf::IntRect(0, boss.getTextureRect().top + 96, 62, 96));
			else
				boss.setTextureRect(sf::IntRect(boss.getTextureRect().left + 64, boss.getTextureRect().top, 62, 96));	
		}

		else if (i == 4 && animation_time > 0.75f)
		{
			animation_time = 0.f;

			boss.setColor(sf::Color(boss.getColor().r, boss.getColor().g, boss.getColor().b, boss.getColor().a - 25));

			if (boss.getColor().a < 25)
			{
				boss.is_drawing = false;
				i = 5;
			}
			
			boss.is_outro = true;

		}

	}
	


}
