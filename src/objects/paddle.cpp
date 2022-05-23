#include "paddle.h"

Paddle::Paddle(GameDataRef data)
    :is_moving(false), is_drawing(false), moving_right(false), velocity(300.f, 0.f), transition_type(3), is_breaking_out(false), death(false), 
    data(data)
{
    setTexture();

    state = &spawn;

    setPosition(sf::Vector2f(WIDTH / 2.f, HEIGHT - 19.f)); 
    shadow.setPosition(getPosition().x + 4, getPosition().y + 4);
}

void Paddle::setTexture()
{
    sf::Sprite::setTexture(data->asset_manager.getTexture("paddle_texture"));
    sf::Sprite::setTextureRect(sf::IntRect(208, 0, 32, 8));
    setOrigin(sf::Vector2f(getGlobalBounds().width / 2.f, getGlobalBounds().height / 2.f));

    shadow.setTexture(*getTexture());
    //sf::Sprite::setTextureRect(getTextureRect());
    shadow.setColor(sf::Color(0, 0, 0, 250));
    shadow.setOrigin(getOrigin());
}

void Paddle::update(const float& dt)
{
    if (is_drawing)
    {
        animation(dt);
        updateShadow();

        if (is_breaking_out)
        {
            is_moving = false;
            if (getGlobalBounds().left < WIDTH)
                move(sf::Vector2f(50.f, 0.f) * dt);
            else
            {
                is_breaking_out = false;
                is_drawing = false;
            }         
        }
    }
}

void Paddle::updateShadow()
{
    shadow.setTextureRect(getTextureRect());
    shadow.setOrigin(getOrigin());
    shadow.setPosition(getPosition().x + 4, getPosition().y + 4);
}

void Paddle::draw()
{
   if (is_drawing)
    {
        data->window.draw(shadow);
        data->window.draw(*this);
    }
}

void Paddle::animation(const float& dt)
{
    state->animation(*this, dt);
}

int Paddle::getCollisionSide(const sf::Sprite& object)
{
    //Sprawdza, w ktore miejsce na palecie uderzyla ball

    if (object.getPosition().y < getPosition().y + getGlobalBounds().height/4.f)
    {
        if (object.getPosition().x <= getGlobalBounds().left + 4.f)                                           
            return 1;           //POMARANCZOWY LEWY
        else if (object.getPosition().x >= getGlobalBounds().left + getGlobalBounds().width - 4.f)      
            return 2;           //POMARANCZOWY PRAWY
        else
        {
            if (object.getPosition().x <= getGlobalBounds().left + 9.f)                                       
                return 3;       //CZERWONY LEWY
            else if (object.getPosition().x >= getGlobalBounds().left + getGlobalBounds().width - 9.f)  
                return 4;       //CZERWONY PRAWY
            else
            {
                if (object.getPosition().x < getPosition().x)         
                    return 5;   //SREBRNY LEWY
                else if (object.getPosition().x > getPosition().x)    
                    return 6;   //SREBRNY PRAWY
                else                                                        
                    return 7;   //SRODEK
            }
        }
    }
    return 0;
}

void Paddle::catchBall(sf::Sprite& ball)
{
    if (ball.getGlobalBounds().left < getGlobalBounds().left)
        ball.setPosition(sf::Vector2f(ball.getPosition().x - 2.f + (getGlobalBounds().left - ball.getGlobalBounds().left), getPosition().y - getGlobalBounds().height / 2.f - ball.getGlobalBounds().height / 2.f));
    else if (ball.getGlobalBounds().left + ball.getGlobalBounds().width > getGlobalBounds().left + getGlobalBounds().width)
        ball.setPosition(sf::Vector2f(ball.getPosition().x + 2.f - ((ball.getGlobalBounds().left + ball.getGlobalBounds().width) - (getGlobalBounds().left + getGlobalBounds().width)), getPosition().y - getGlobalBounds().height / 2.f - ball.getGlobalBounds().height / 2.f));
    else
        ball.setPosition(sf::Vector2f(ball.getPosition().x, getPosition().y - getGlobalBounds().height / 2.f - ball.getGlobalBounds().height / 2.f));

}
