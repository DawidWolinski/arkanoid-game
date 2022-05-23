#pragma once
#include "boss_animation.h"

class BossShooting : public BossAnimation
{
public:

	BossShooting();
	void animation(Boss& boss, Paddle& paddle, const float& dt) override;
	void reset();

private:

	sf::Vector2f getDirection(sf::Vector2f projectile_position, Paddle& paddle);

	float animation_time;
	float shoot_time;
	sf::RectangleShape projectile;
	
};

