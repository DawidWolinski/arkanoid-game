#include "boss_shooting.h"
#include "../objects/boss.h"
#include "../objects/paddle.h"


BossShooting::BossShooting()
	: animation_time(0.f), shoot_time(0.f)
{
}

void BossShooting::animation(Boss& boss, Paddle& paddle, const float& dt)
{
	animation_time += 10.f * dt;
	shoot_time += 10.f * dt;


	if (animation_time > 30.f)
	{
		animation_time = 0.f;

		boss.state = &boss.closing;
	}
	else if(shoot_time > 5.f)
	{
		shoot_time = 0.f;

		boss.projectiles.push_back(std::make_tuple<std::unique_ptr<sf::Sprite>, std::unique_ptr<sf::Vector2f>>(std::make_unique<sf::Sprite>(boss.projectile), std::make_unique<sf::Vector2f>(getDirection(boss.getPosition(), paddle)), 0));
		std::get<0>(boss.projectiles.back())->setPosition(boss.getPosition());
	}	
}

sf::Vector2f BossShooting::getDirection(sf::Vector2f projectile_position, Paddle& paddle)
{
	float length = std::sqrt(projectile_position.x * projectile_position.x + projectile_position.y * projectile_position.y);

	sf::Vector2f distance = paddle.getPosition() - projectile_position;

	distance.x /= length;
	distance.y /= length;

	return distance;
}

void BossShooting::reset()
{
	animation_time = 0.f;
	shoot_time = 0.f;
}