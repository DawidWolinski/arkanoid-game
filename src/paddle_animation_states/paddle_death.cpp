#include "paddle_death.h"
#include "../objects/paddle.h"

PaddleDeath::PaddleDeath()
    : i(0)
{
    getFrames();
}

void PaddleDeath::animation(Paddle& paddle, const float& dt)
{
    animation_time += 10.f * dt;
    if (animation_time > 0.9f && !paddle.death)
    {
        animation_time = 0.f;
        paddle.is_moving = false;

        paddle.setTextureRect(frames[i]);
        paddle.setOrigin(sf::Vector2f(paddle.getGlobalBounds().width / 2.f, paddle.getGlobalBounds().height / 2.f));

        i++;
        if (i >= frames.size())
        {
            i = 0;
            paddle.death = true;
            paddle.is_drawing = false;
            paddle.setPosition(WIDTH + paddle.getGlobalBounds().width, paddle.getPosition().y);       
        }
            
    }
}

void PaddleDeath::getFrames()
{
    if (frames.empty())
    {
        frames.push_back(sf::IntRect(176, 0, 32, 8));
        frames.push_back(sf::IntRect(176, 8, 32, 8));
        frames.push_back(sf::IntRect(176, 16, 32, 8));
        frames.push_back(sf::IntRect(24, 76, 32, 24));
        frames.push_back(sf::IntRect(69, 76, 37, 24));        
        frames.push_back(sf::IntRect(116, 76, 41, 24));
        frames.push_back(sf::IntRect(163, 79, 44, 21)); 
    } 
}