#include "final_level.h"
#include "../paddle_animation_states/paddle_animation.h"

BossState::BossState(GameDataRef data, int hp)
    : wait_time(0.f), hp(hp), score(data->upper_interface.getScore()), NoDirection(Direction::NoDirection), data(data)
{

}

void BossState::initialise()
{
    data->asset_manager.loadTexture("boss_texture", BOSS_TEXTURE_FILE);
    data->asset_manager.loadTexture("paddle_texture", PADDLE_TEXTURE_FILE);
    data->asset_manager.loadTexture("bonus_texture", BONUS_TEXTURE_FILE);
    data->asset_manager.loadTexture("projectile_texture", PROJECTILE_TEXTURE_FILE);

    loadSounds();

    level = std::make_unique<Level>(data);
    player = std::make_unique<Paddle>(data);
    background = std::make_unique<sf::Sprite>();
    frame = std::make_unique<sf::Sprite>();
    balls.push_back(std::make_unique<Ball>(data));
    menu = std::make_unique<GameplayMenu>(data, true);

    level->createLevel(background, frame, hp_sprite);
    boss = std::make_unique<Boss>(data, frame->getPosition());
}

void BossState::loadSounds()
{
    data->asset_manager.loadSound("boss_bounce_sound", BOSS_BOUNCE_SOUND_FILE);
    data->asset_manager.loadSound("boss_death_sound", BOSS_DEATH_SOUND_FILE);

    boss_bounce_sound.setBuffer(data->asset_manager.getSound("boss_bounce_sound"));
    boss_bounce_sound.setVolume(25.f);
    paddle_bounce_sound.setBuffer(data->asset_manager.getSound("paddle_bounce_sound"));
    paddle_bounce_sound.setVolume(25.f);
    death_sound.setBuffer(data->asset_manager.getSound("paddle_death_sound"));
    death_sound.setVolume(25.f);

    data->asset_manager.loadMusic("outro_sound", OUTRO_SOUND_FILE);
    data->asset_manager.getMusic("outro_sound")->setVolume(25.f);
}

void BossState::handleEvents(const float& dt)
{
    // Paddle movement
    if (player->is_moving && !menu->active)
    {
        // Move left
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) && !player->moving_right)
        {
            if (player->getPosition().x - player->getGlobalBounds().width / 2.f > 9.f)
                player->move(-player->velocity * dt);
            else
                player->setPosition(sf::Vector2f(8.f + player->getGlobalBounds().width / 2.f, player->getPosition().y));
        }
        // Move right
        else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) || (sf::Keyboard::isKeyPressed(sf::Keyboard::D)))
        {
            if (player->getPosition().x + player->getGlobalBounds().width / 2.f < WIDTH - 9.f)
                player->move(player->velocity * dt);
            else
                player->setPosition(sf::Vector2f(WIDTH - 8.f - player->getGlobalBounds().width / 2.f, player->getPosition().y));

            player->moving_right = true;
        }
    }

    // Rest
    while (data->window.pollEvent(data->event))
    {
        switch (data->event.type)
        {
        case sf::Event::Closed:
            data->window.close();
            break;
        case sf::Event::Resized:
            data->window.setPosition(sf::Vector2i(data->window.getPosition().x - int(WIDTH) / 2, data->window.getPosition().y - int(HEIGHT) / 2));
            break;
        case sf::Event::KeyPressed:
            if (data->event.key.code == sf::Keyboard::Escape || data->event.key.code == sf::Keyboard::P)
                menu->active = !menu->active;
            if ((data->event.key.code == sf::Keyboard::S || data->event.key.code == sf::Keyboard::Down) && menu->active)
                menu->changeOption(0);
            if ((data->event.key.code == sf::Keyboard::W || data->event.key.code == sf::Keyboard::Up) && menu->active)
                menu->changeOption(1);
            if (data->event.key.code == sf::Keyboard::Enter && menu->active)
            {
                switch (menu->option_index)
                {
                case 0:
                    menu->active = false;
                    break;
                case 1:
                    data->upper_interface.reset();
                    data->state_machine.addState(StateRef(new MenuState(data)));
                    break;
                }
            }    
            break;
        case sf::Event::KeyReleased:
            if (data->event.key.code == sf::Keyboard::Right || data->event.key.code == sf::Keyboard::D)
                player->moving_right = false;
            break;
        }
    }
}

void BossState::checkCollisions()
{
    // Ball collision
    for (auto& ball : balls)
    {
        if (ball->getGlobalBounds().intersects(boss->hitbox.getGlobalBounds()))
        {
            Direction bounceDirection = boss->whichSide(*ball, ball->velocity);

            if (bounceDirection != NoDirection)
            {
                ball->bounce(bounceDirection);
                boss->handleHit();
                data->upper_interface.increaseScore(1000);
                if(boss->state == &boss->death)
                    while (!balls.empty()) balls.pop_back();
                boss_bounce_sound.play();
            }
        }

        // With paddle
        if (ball->getGlobalBounds().intersects(player->getGlobalBounds()))
        {
            int collision_side = player->getCollisionSide(*ball);
            if (collision_side != 0)
            {
                ball->bouncePaddle(collision_side);
                ball->bounced = false;
            }
        }
        else if (!ball->bounced)
        {
            paddle_bounce_sound.play();
            ball->bounced = true;
        }
            

        // With the bottom
        if (ball->getPosition().y > HEIGHT + ball->getGlobalBounds().height)
        {
            while (!balls.empty()) balls.pop_back();
            hp--;
            player->state = &player->death_animation;
            death_sound.play();
        }
    }  

    // Collision of paddle with projectile
    for (unsigned int i = 0; i < boss->projectiles.size(); i++)
    {
        
        if (std::get<0>(boss->projectiles[i])->getGlobalBounds().intersects(player->getGlobalBounds()) && player->is_moving)
        {
            boss->projectiles.erase(boss->projectiles.begin() + i);
            while (!balls.empty()) balls.pop_back();            
            if(level->start) level->resetStart();
            hp--;
            boss->state = &boss->closing;
            player->state = &player->death_animation;
            death_sound.play();
        }
        else if (std::get<0>(boss->projectiles[i])->getPosition().y > HEIGHT)
            boss->projectiles.erase(boss->projectiles.begin() + i);
            
    }

}

void BossState::checkGameState(const float& dt)
{
    // If boss or player died
    if (player->getGlobalBounds().left >= WIDTH)
    {
        if (wait_time == 0.f)
        {
            while (!balls.empty()) balls.pop_back();
            player->is_drawing = false;
        }

        wait_time += dt;
        if (wait_time > 1.f)
        {
            if (hp == 0)
                data->state_machine.addState(StateRef(new TossCoin(level->number + 1, data)));               
            else if (player->death)
                level->start = true;

            reset();
        }
    }
    else if (!boss->is_drawing)
    {
        wait_time += dt;

        if (boss->is_outro && wait_time > 1.f)
        {
            data->asset_manager.getMusic("outro_sound")->play();
            boss->is_outro = false;
        }
    }
}

void BossState::reset()
{
    balls.push_back(std::make_unique<Ball>(data));
    player.reset();
    player = std::make_unique<Paddle>(data);
    wait_time = 0.f;
}

void BossState::update(const float& dt)
{
    if (!menu->active)
    {
        if (level->start) level->startLevel(player, balls.back(), dt);

        player->update(dt);

        for (auto& ball : balls) 
            ball->update(dt, sf::Vector2f(WIDTH, HEIGHT));

        boss->update(dt, player);

        checkCollisions();

        checkGameState(dt);
    }
    else
        menu->update(dt);

    data->upper_interface.update(dt);
}

void BossState::draw(const float& dt)
{
    data->window.clear();

    data->window.draw(*background);

    data->window.draw(*frame);

    boss->draw();

    for (auto& ball : balls) 
        ball->drawShadow();

    player->draw();

    for (auto& ball : balls) 
        ball->draw();

    if (hp > 1)
        for (int i = 0; i < hp - 1; i++)
            data->window.draw(*hp_sprite[i]);

    data->upper_interface.draw();

    level->draw();

    menu->draw();

    data->window.display();
}