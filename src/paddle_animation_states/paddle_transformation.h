#pragma once
#include "paddle_animation.h"

#include <stack>
#include <vector>

class PaddleTransformation : public PaddleAnimation
{
public:

	PaddleTransformation();
	void animation(Paddle& paddle, const float& dt) override;
	void reset();

private:

	float animation_time;
	std::stack<sf::IntRect> frames;
	std::vector<std::stack<sf::IntRect>> transformation_frames;
	bool cleared;

	void getFrames();
};

