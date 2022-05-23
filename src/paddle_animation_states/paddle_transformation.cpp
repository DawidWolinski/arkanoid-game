#include "paddle_transformation.h"
#include "../objects/paddle.h"


PaddleTransformation::PaddleTransformation()
    : cleared(true)
{
    std::stack<sf::IntRect> temporary;
    transformation_frames.push_back(temporary);
    transformation_frames.push_back(temporary);
    getFrames();
}

void PaddleTransformation::animation(Paddle& paddle, const float& dt)
{
    animation_time += 10.f * dt;
    if (animation_time > 0.4f)
    {
        animation_time = 0.f;

        // Removes current transformation
        if (!frames.empty() && !cleared)
        {           
            paddle.setTextureRect(frames.top());
            frames.pop();
            paddle.setOrigin(sf::Vector2f(paddle.getGlobalBounds().width / 2.f, paddle.getGlobalBounds().height / 2.f));
            if (frames.empty()) cleared = true;
        }
        // Transformation
        else if (paddle.transition_type != 0)
        {      
            frames.push(std::move(transformation_frames[paddle.transition_type - 1].top()));
            transformation_frames[paddle.transition_type - 1].pop();
            paddle.setTextureRect(frames.top());
            paddle.setOrigin(sf::Vector2f(paddle.getGlobalBounds().width / 2.f, paddle.getGlobalBounds().height / 2.f));         
            if (transformation_frames[paddle.transition_type - 1].empty())
            {
                paddle.state = &paddle.standard;
                cleared = false;
                getFrames();
            }
        }
        else
            paddle.state = &paddle.standard;
    }
}

void PaddleTransformation::getFrames()
{

    // Laser
    if (transformation_frames[0].empty())
    {
        transformation_frames[0].push(sf::IntRect(144, 0, 32, 8));
        for (int i = 8; i >= 0; i--)
            transformation_frames[0].push(sf::IntRect(112, i * 8, 32, 8));       

        transformation_frames[0].push(sf::IntRect(32, 0, 32, 8));
    }

    // Enlarge
    if (transformation_frames[1].empty())
    {
        transformation_frames[1].push(sf::IntRect(64, 0, 48, 8));
        transformation_frames[1].push(sf::IntRect(208, 34, 44, 8));
        transformation_frames[1].push(sf::IntRect(210, 42, 40, 8));
        transformation_frames[1].push(sf::IntRect(212, 50, 36, 8));
        transformation_frames[1].push(sf::IntRect(213, 58, 34, 8));
        transformation_frames[1].push(sf::IntRect(32, 0, 32, 8));    
    }
}

void PaddleTransformation::reset()
{
    while (!frames.empty())
        frames.pop();

    for (auto frames : transformation_frames)
        while (!frames.empty())
            frames.pop();

    cleared = true;

    getFrames();
}
