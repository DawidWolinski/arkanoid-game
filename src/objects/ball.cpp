#include "ball.h"



Ball::Ball(GameDataRef data)
    :is_moving(false), speed(150.f), pi(atan(1.f) * 4.f), distane_from_paddle(0.f), is_drawing(false), bounced(true),
    Down(Direction::Down), Up(Direction::Up), Left(Direction::Left), Right(Direction::Right),
    random_number(0, 2), data(data)
{
    velocity = sf::Vector2f(-speed, speed);
    setTexture();
    
    setPosition(sf::Vector2f(WIDTH, HEIGHT));
    changeAngle(140.f);
    
}

void  Ball::setTexture()
{
    sf::Sprite::setTexture(data->asset_manager.getTexture("paddle_texture"));
    sf::Sprite::setTextureRect(sf::IntRect(0, 40, 5, 4));
    setOrigin(sf::Vector2f(getGlobalBounds().width / 2.f, getGlobalBounds().height / 2.f));
    shadow.setTexture(*getTexture());
    shadow.setTextureRect(sf::IntRect(4, 44, 5, 4));
    shadow.setOrigin(getOrigin());
}

void Ball::update(const float& dt, const sf::Vector2f& window_size)
{
    movement(dt, window_size);
}

void Ball::movement(const float& dt, const sf::Vector2f& window_size)
{
    if (is_moving)
    {
        move(velocity * dt);
        if (getPosition().x + getGlobalBounds().width / 2.f >= window_size.x - 8.f) bounce(Left);
        if (getPosition().x - getGlobalBounds().width / 2.f <= 8.f) bounce(Right);
        if (getPosition().y - getGlobalBounds().height / 2.f <= 24.f) bounce(Down);
    }

    shadow.setPosition(getPosition().x + 4, getPosition().y + 4);
}

void Ball::draw()
{
    if (is_drawing) 
    { 
        data->window.draw(*this); 
    }
}

void Ball::drawShadow()
{
    if(is_drawing)
        data->window.draw(shadow);
}

void Ball::changeAngle(const float& degrees)
{
    this->degrees = degrees;
    float radians = 2 * pi * (degrees / 360);

    if (velocity.x >= 0)
        velocity.x = speed * cos(radians);
    else
        velocity.x = -speed * cos(radians);
    if (velocity.y >= 0)
        velocity.y = speed * sin(radians);
    else
        velocity.y = -speed * sin(radians);
}

void Ball::bounce(Direction bounce_direction, const float& degrees)
{
    if (degrees != 0)
        changeAngle(degrees);
 
    switch (bounce_direction)
    {
    case Direction::Right:
        velocity.x = std::abs(velocity.x);
        break;
    case Direction::Left:
        velocity.x = -std::abs(velocity.x);
        break;
    case Direction::Down:
        velocity.y = std::abs(velocity.y);
        break;
    case Direction::Up:
        velocity.y = -std::abs(velocity.y);
        break;
    }
}

void Ball::bouncePaddle(const int& collision_side)
{
    // Paddle is divided into different segments (Orange, Red, Silver, Middle)
    // Paddle bounces off at a different angle depending on which segment of the paddle it hit
    switch (collision_side)
    {
    case 1:     // ORANGE LEFT
        bounce(Left, 30);
        bounce(Up);
        break;
    case 2:     // ORANGE RIGHT
        bounce(Right, 30);
        bounce(Up);
        break;
    case 3:     // RED LEFT
        bounce(Left, 45);
        bounce(Up);
        break;
    case 4:     // RED RIGHT
        bounce(Right, 45);
        bounce(Up);
        break;
    case 5:     // SILVER LEFT
        bounce(Left, 65);
        bounce(Up);
        break;
    case 6:     // SILVER RIGHT
        bounce(Right, 65);
        bounce(Up);
        break;
    case 7:     // MIDDLE
        changeAngle(65);
        bounce(Up);
        break;
    default:
        break;
    }
}

void Ball::placeOnPaddle(const sf::Sprite& paddle)
{
    setPosition(paddle.getPosition().x, paddle.getPosition().y - paddle.getGlobalBounds().height / 2.f - getGlobalBounds().height / 2.f);
}

void Ball::increaseSpeed()
{
    if (speed <= 300.f && random_number(data->generator) == 1)
    {
        speed += 5.f;
        velocity.x = speed * cos(std::atan2(velocity.y, velocity.x));
        velocity.y = speed * sin(std::atan2(velocity.y, velocity.x));
    }
        
}

void Ball::decreaseSpeed(float decreaseAmount)
{
    speed -= decreaseAmount;
    if (speed < 150.f) 
        speed = 150.f;

    velocity.x = speed * cos(std::atan2(velocity.y, velocity.x));
    velocity.y = speed * sin(std::atan2(velocity.y, velocity.x));
}