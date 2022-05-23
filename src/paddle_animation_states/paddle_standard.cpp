#include "paddle_standard.h"
#include "../objects/paddle.h"

PaddleStandard::PaddleStandard()
    : animation_time(0.f), paddle_width(0)
{

}

void PaddleStandard::animation(Paddle& paddle, const float& dt)
{
    animation_time += 5.f * dt;
    if (animation_time > 0.6f)
    {
        animation_time = 0.f;

        if (paddle.getTextureRect().left == 64)
            paddle_width = 48;
        else
            paddle_width = 32;

        if (paddle.getTextureRect().top == 40)
            paddle.setTextureRect(sf::IntRect(paddle.getTextureRect().left, 0, paddle_width, 8));
        else
            paddle.setTextureRect(sf::IntRect(paddle.getTextureRect().left, paddle.getTextureRect().top + 8, paddle_width, 8));
    }
}