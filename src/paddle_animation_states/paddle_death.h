#pragma once
#include "paddle_animation.h"

class PaddleDeath : public PaddleAnimation
{
public:

	PaddleDeath();
	void animation(Paddle& paddle, const float& dt) override;

private:


	float animation_time;
	std::vector<sf::IntRect> frames;
	unsigned int i;

	void getFrames();
};

