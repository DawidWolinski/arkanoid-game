#include "boss.h"

Boss::Boss(GameDataRef data, sf::Vector2f frame_position)
	: is_drawing(true), hp(16), hit_time(0.f), death_time(0.f), projectile_time(0.f), multiplier(1), top(144), data(data), is_outro(false),
    Down(Direction::Down), Up(Direction::Up), Left(Direction::Left), Right(Direction::Right), NoDirection(Direction::NoDirection)
{
    death_sound.setBuffer(data->asset_manager.getSound("boss_death_sound"));
    death_sound.setVolume(25.f);

	background.setSize(sf::Vector2f(64.f, 96.f));
	background.setFillColor(sf::Color::Black);
	background.setOrigin(sf::Vector2f(background.getGlobalBounds().width / 2.f, background.getGlobalBounds().height / 2.f));
	background.setPosition(sf::Vector2f(frame_position.x + 8.f + background.getGlobalBounds().width / 2.f, frame_position.y + 23.f + background.getGlobalBounds().height / 2.f));

	setTexture(data->asset_manager.getTexture("boss_texture"));
	setTextureRect(sf::IntRect(0, 144, 62, 96));
	setOrigin(sf::Vector2f(getGlobalBounds().width / 2.f, getGlobalBounds().height / 2.f));
	setPosition(background.getPosition());

	hitbox.setSize(sf::Vector2f(46.f, 90.f));
	hitbox.setOrigin(sf::Vector2f(hitbox.getGlobalBounds().width / 2.f, hitbox.getGlobalBounds().height / 2.f));
	hitbox.setPosition(sf::Vector2f(getPosition().x, getPosition().y));


    projectile.setTexture(data->asset_manager.getTexture("projectile_texture"));
    projectile.setTextureRect(sf::IntRect(7, 63, 7, 8));

    projectile_frames.push_back(sf::IntRect(7, 63, 7, 8));
    projectile_frames.push_back(sf::IntRect(26, 62, 7, 8));
    projectile_frames.push_back(sf::IntRect(45, 62, 7, 8));
    projectile_frames.push_back(sf::IntRect(67, 61, 7, 8));
    projectile_frames.push_back(sf::IntRect(88, 61, 7, 8));
    projectile_frames.push_back(sf::IntRect(105, 61, 7, 8));
    projectile_frames.push_back(sf::IntRect(7, 83, 7, 8));
    projectile_frames.push_back(sf::IntRect(26, 83, 7, 8));
    projectile_frames.push_back(sf::IntRect(48, 83, 8, 7));
    projectile_frames.push_back(sf::IntRect(68, 83, 7, 8));
    projectile_frames.push_back(sf::IntRect(87, 82, 8, 7));


    state = &opening;
}

Direction Boss::whichSide(const sf::Sprite& object, const sf::Vector2f& velocity)
{
    // Checks ball collision side
    float dx = std::abs(object.getPosition().x - hitbox.getPosition().x) - hitbox.getGlobalBounds().width / 2.f;
    float dy = std::abs(object.getPosition().y - hitbox.getPosition().y) - hitbox.getGlobalBounds().height / 2.f;

        if (dy < dx)
        {
            if (object.getPosition().x > hitbox.getPosition().x && velocity.x < 0) 
                return Right;
            else if (object.getPosition().x < hitbox.getPosition().x && velocity.x > 0) 
                return Left;
        }
        else
        {
            if (object.getPosition().y > hitbox.getPosition().y && velocity.y < 0) 
                return Down;
            else if (object.getPosition().y < hitbox.getPosition().y && velocity.y > 0) 
                return Up;
        }

    return NoDirection;
}

void Boss::update(const float& dt, std::unique_ptr<Paddle>& player)
{
    animation(dt, player);

    // Projectile movement
    for (auto& projectile : projectiles)
        std::get<0>(projectile)->move(*std::get<1>(projectile) * dt * 100.f);
}

void Boss::draw()
{
    data->window.draw(background);
	if (is_drawing)
	    data->window.draw(*this);

    for (auto& projectile : projectiles)
        data->window.draw(*std::get<0>(projectile));
}

void Boss::animation(const float& dt, std::unique_ptr<Paddle>& player)
{
    state->animation(*this, *player, dt);
    if (!projectiles.empty())
        projectileAnimation(dt);

    if (hp > 0)
    {
        hitAnimation(dt);
        deathAnimation(dt);
    }
}

void Boss::handleHit()
{
    hp--;
    
    // Boss starts blinking at 3 HP
    if (hp == 3)
        setTextureRect(sf::IntRect(getTextureRect().left, 336, 62, 96));

    if(top != 240) 
        top = getTextureRect().top;

    // Hit animation
    if(hp > 0)
        setTextureRect(sf::IntRect(getTextureRect().left, 240, 62, 96));
    else
    {
        while (!projectiles.empty()) projectiles.pop_back();
        state = &death;
        death_sound.play();
    }

    hit_time = 0.f;
}

void Boss::hitAnimation(const float& dt)
{
    if (getTextureRect().top == 240)
    {
        hit_time += 10.f * dt;

        if (hit_time > 0.8f)
        {
            hit_time = 0.f;
            setTextureRect(sf::IntRect(getTextureRect().left, top, 62, 96));
        }
    }
}

void Boss::deathAnimation(const float& dt)
{
    if (getTextureRect().top != 240 && hp < 4)
    {
        death_time += 10.f * dt;
        if (death_time > 1.5f)
        {
            death_time = 0.f;

            setTextureRect(sf::IntRect(getTextureRect().left, getTextureRect().top + 96 * multiplier, 62, 96));

            if (getTextureRect().top == 528)
                multiplier = -1;
            else if (getTextureRect().top == 336)
                multiplier = 1;
        }
    }
}

void Boss::projectileAnimation(const float& dt)
{
    projectile_time += 10.f * dt;

    if (projectile_time >= 0.5f)
    {
        for (auto& projectile : projectiles)
        {
            std::get<2>(projectile)++;
            if (std::get<2>(projectile) == projectile_frames.size())
                std::get<2>(projectile) = 0; 

            std::get<0>(projectile)->setTextureRect(projectile_frames[std::get<2>(projectile)]);
            std::get<0>(projectile)->setOrigin(sf::Vector2f(std::get<0>(projectile)->getGlobalBounds().width / 2.f, std::get<0>(projectile)->getGlobalBounds().height / 2.f));
        }

        projectile_time = 0.f;
    }
}