#include "bonus.h"

PowerUp::PowerUp(GameDataRef data)
    :   type(10), active_type(10), is_moving(false), is_drawing(true), is_break_out(false),
        velocity(sf::Vector2f(0.f, 70.f)), laser_velocity(sf::Vector2f(0.f, -300.f)),
        catchTime(0.f), catchLimit(5.f), random_number(0, 6), random_number_break_out(0, 1), data(data)
{
    setTexture();
    setPosition(sf::Vector2f(WIDTH / 2.f, HEIGHT + getGlobalBounds().height/2.f + 1.f));
    shadow.setPosition(getPosition().x + 4, getPosition().y + 4);
    break_out.setPosition(WIDTH - break_out.getGlobalBounds().width, HEIGHT - break_out.getGlobalBounds().height);
    laser_distance = std::abs(laser_velocity.y) / 5.f;
    enlarge_sound.setBuffer(data->asset_manager.getSound("enlarge_sound"));
    enlarge_sound.setVolume(25.f);
    laser_sound.setBuffer(data->asset_manager.getSound("laser_sound"));
    laser_sound.setVolume(25.f);

}

void PowerUp::setTexture()
{
    sf::Sprite::setTexture(data->asset_manager.getTexture("bonus_texture"));
    sf::Sprite::setTextureRect(sf::IntRect(0, 0, 16, 7));
    setOrigin(getGlobalBounds().width / 2.f, getGlobalBounds().height / 2.f);

    shadow.setTexture(*getTexture());
    shadow.setTextureRect(getTextureRect());
    shadow.setColor(sf::Color(0, 0, 0, 250));
    shadow.setOrigin(getOrigin());

    break_out.setTexture(data->asset_manager.getTexture("background_texture"));
    break_out.setTextureRect(sf::IntRect(924, 256, 8, 40));
}

void PowerUp::drop(const sf::Vector2f& position)
{
    if (!is_moving)
    {
       // Selecting random power up
       // 0 - Slow, 1 - Catch, 2 - Laser, 3 - Enlarge, 4 - Disruption, 5 - Break (out), 6 - Player (HP)

       int temporary = 0;
        is_moving = true;
        do
        {
             temporary = random_number(data->generator);

             // Break out has lower chance of dropping 
             if (temporary == 5)
                 if (random_number_break_out(data->generator) == 0)
                     temporary = active_type;
                     
        } while (temporary == active_type || temporary == type || temporary == 5 && break_out.getTextureRect().left != 924);
        type = temporary;
        setPosition(position);
        sf::Sprite::setTextureRect(sf::IntRect(0, type * 8, 16, 7));
    }
}

void PowerUp::update(const float& dt)
{
    if (is_moving)
    {
        if (getPosition().y < HEIGHT + getGlobalBounds().height / 2.f)
        {
            animation(dt);
            move(velocity * dt);
            shadow.setPosition(getPosition().x + 2, getPosition().y + 2);
        }
        else
            is_moving = false;
    }
    
    breakOutAnimation(dt);
    updateLasers(dt);
}

void PowerUp::activatePowerUp(std::unique_ptr<Paddle>& player, std::unique_ptr<Ball>& pilka)
{
    setPosition(sf::Vector2f(WIDTH / 2.f, HEIGHT + getGlobalBounds().height / 2.f + 1.f));
    shadow.setPosition(getPosition().x + 4, getPosition().y + 4);
    is_moving = false;
    active_type = type;
    pilka->is_moving = true;

    if (type == 2 || type == 3)
        player->transition_type = type - 1;
    else player->transition_type = 0;

    player->state = &player->transformation;
    catchTime = 0.f;
    data->upper_interface.increaseScore(1000);
    if (type == 3) 
        enlarge_sound.play();
}

void PowerUp::draw()
{
    if (is_drawing)
    {
        data->window.draw(shadow);
        data->window.draw(*this);
    }
 
    data->window.draw(break_out);

    for (auto& laser : lasers)
        data->window.draw(*laser);
}

void PowerUp::animation(const float& dt)
{
    animation_time += 10.f * dt;
    if (animation_time > 1.f)
    {
        animation_time = 0.f;

        if (getTextureRect().left == 112)
            sf::Sprite::setTextureRect(sf::IntRect(0, getTextureRect().top, getTextureRect().width, getTextureRect().height));
        else
            sf::Sprite::setTextureRect(sf::IntRect(getTextureRect().left + 16, type * 8, 16, 7));
    }
}

void PowerUp::shootLaser(const sf::Vector2f& paddle_position)
{
    if (lasers.empty() || paddle_position.y > laser_distance + lasers.back()->getPosition().y)
    {
        lasers.push_back(std::make_unique<Laser>(data, paddle_position));
        laser_sound.play();
    }   
}

void PowerUp::updateLasers(const float& dt)
{
    for (unsigned int i = 0; i < lasers.size(); i++)
    {
        lasers[i]->update(dt);
        if(lasers[i]->remove)
            lasers.erase(lasers.begin() + i);
    }
        
}

void PowerUp::breakOutAnimation(const float& dt)
{
    if (is_break_out)
    {
        break_out_animation_time += 10.f * dt;
        if (break_out_animation_time > 0.5f)
        {
            break_out_animation_time = 0.f;

            if (break_out.getTextureRect().left == 980)   // When it's open
                break_out.setTextureRect(sf::IntRect(break_out.getTextureRect().left - 16, break_out.getTextureRect().top, break_out.getTextureRect().width, break_out.getTextureRect().height));
            else                                                // When it's open and is being opened
                break_out.setTextureRect(sf::IntRect(break_out.getTextureRect().left + 8, break_out.getTextureRect().top, break_out.getTextureRect().width, break_out.getTextureRect().height));
        }
    }   
}


// Laser
Laser::Laser(GameDataRef data, sf::Vector2f paddle_position)
    :is_collision(false), remove(false), animation_time(0.f), velocity(sf::Vector2f(0.f, -300.f)), data(data)
{
    setTexture(data->asset_manager.getTexture("paddle_texture"));
    setTextureRect(sf::IntRect(152, 48, 16, 8));
    setOrigin(getGlobalBounds().width / 2.f, getGlobalBounds().height / 2.f);
    setPosition(paddle_position);
}

void Laser::update(const float& dt)
{
    if (!is_collision)      // Laser moves until it collides with a brick or a ceiling
    {
        move(velocity * dt);
        if (getPosition().y <= 30.f)
            is_collision = true;
    }
    else if (getTextureRect().top == 48)        // First animation frame of laser explosion
    {
        setTextureRect(sf::IntRect(153, 56, 14, 3));
        setPosition(sf::Vector2f(getPosition().x, getPosition().y - getGlobalBounds().height - 2.f));
        setOrigin(getGlobalBounds().width / 2.f, getGlobalBounds().height / 2.f);
    }
    else
    {
        animation_time += dt * 10.f;

        if (animation_time >= 0.5f)
        {
            if (getTextureRect().top == 56)       // Second animation frame of laser explosion
                setTextureRect(sf::IntRect(152, 64, 16, 3));
            else                                        // Lasers are removed in PowerUp::updateLasers()
                remove = true;          

            setOrigin(getGlobalBounds().width / 2.f, getGlobalBounds().height / 2.f);
            animation_time = 0.f;
        }
    }
}