#include "paddle_spawn.h"
#include "../objects/paddle.h"

void PaddleSpawn::animation(Paddle& paddle, const float& dt)
{
    animation_time += 10.f * dt;
    if (animation_time > 0.5f)
    {
        animation_time = 0.f;

        if (paddle.getTextureRect().left != 0)
            paddle.setTextureRect(sf::IntRect(0, 0, 32, 8));

        if (paddle.getTextureRect().top == 32)
        {
            paddle.setTextureRect(sf::IntRect(32, 0, 32, 8));
            paddle.state = &paddle.standard;
        }
        else
            paddle.setTextureRect(sf::IntRect(paddle.getTextureRect().left, paddle.getTextureRect().top + 8, 32, 8));
    }
}
