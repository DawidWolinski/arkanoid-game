#include "gameplay.h"
#include <iostream>

GameplayState::GameplayState(GameDataRef data, bool load_save, const int& which_level)
    : wait_time(0.f), hp(2), load_save(load_save), which_level(which_level), bonus_hp_count(0), NoDirection(Direction::NoDirection),
    random_number(0, 2), data(data)   
{
    
}

void GameplayState::initialise()
{
    // Sounds
    setSounds();
    
    // Objects
    level = std::make_unique<Level>(data);
    player = std::make_unique<Paddle>(data);
    background = std::make_unique<sf::Sprite>();
    balls.push_back(std::make_unique<Ball>(data));
    power_up = std::make_unique<PowerUp>(data);
    menu = std::make_unique<GameplayMenu>(data);

    // Creates current level
    if (load_save)
        loadSave();
    else
        level->createLevel(bricks, background, hp_sprite, which_level);
}

void GameplayState::setSounds()
{
    // Number of ball bounce off sound is limited (so they don't stack)
    paddle_bounce_sound = std::make_pair(sf::Sound(), sf::Clock());
    brick_bounce_sound = std::make_pair(sf::Sound(), sf::Clock());
    gold_brick_bounce_sound = std::make_pair(sf::Sound(), sf::Clock());

    death_sound.setBuffer(data->asset_manager.getSound("paddle_death_sound"));
    death_sound.setVolume(25.f);

    paddle_bounce_sound.first.setBuffer(data->asset_manager.getSound("paddle_bounce_sound"));
    paddle_bounce_sound.first.setVolume(25.f);

    brick_bounce_sound.first.setBuffer(data->asset_manager.getSound("brick_bounce_sound"));
    brick_bounce_sound.first.setVolume(25.f);

    gold_brick_bounce_sound.first.setBuffer(data->asset_manager.getSound("gold_brick_bounce_sound"));
    gold_brick_bounce_sound.first.setVolume(25.f);

    hp_sound.setBuffer(data->asset_manager.getSound("hp_sound"));
    hp_sound.setVolume(25.f);
}

void GameplayState::handleEvents(const float& dt)
{
    // Player movement
    if (player->is_moving && !menu->active)
    {
        // To the left
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) && !player->moving_right)
        {
            if (player->getPosition().x - player->getGlobalBounds().width / 2.f > 9.f)
                player->move(-player->velocity * dt);
            else
                player->setPosition(sf::Vector2f(8.f + player->getGlobalBounds().width / 2.f, player->getPosition().y));
        }
        // To the right
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            if (player->getPosition().x + player->getGlobalBounds().width / 2.f < WIDTH - 9.f)
                player->move(player->velocity * dt);
            else if (power_up->is_break_out)
            {
                player->move(player->velocity * dt);
                if(player->getPosition().x + player->getGlobalBounds().width / 2.f > WIDTH - 6.f)
                    breakingOutAnimation();
            }              
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
            {
                menu->active = !menu->active;
                if (menu->active)
                {
                    for (auto& pilka : balls)
                        if (pilka->velocity.y < 0.f) menu->move_up = true;
                    menu->grayOutOption();
                }
                else
                    menu->move_up = false;       
            }                
            if (data->event.key.code == sf::Keyboard::S || data->event.key.code == sf::Keyboard::Down)
                if(menu->active)
                    menu->changeOption(0);
            if (data->event.key.code == sf::Keyboard::W || data->event.key.code == sf::Keyboard::Up)
                if(menu->active)
                    menu->changeOption(1);
            if (data->event.key.code == sf::Keyboard::Enter && menu->active)
            {
                switch (menu->option_index)
                {
                case 0:
                    menu->active = false;
                    menu->move_up = false;
                    break;
                case 1:
                    saveGame();
                    data->upper_interface.reset();
                    data->state_machine.addState(StateRef(new MenuState(data)));
                    break;
                case 2:
                    data->upper_interface.reset();
                    data->state_machine.addState(StateRef(new MenuState(data)));
                    break;
                }                            
            }                              
            if (data->event.key.code == sf::Keyboard::Space)
                spacePressed();   
                
            break;

        case sf::Event::KeyReleased:
            if (data->event.key.code == sf::Keyboard::Right || data->event.key.code == sf::Keyboard::D)
                player->moving_right = false;
            break;
        }
    } 
}

void GameplayState::spacePressed()
{
    // When Catch power up is active
    if (power_up->active_type == 1 && !balls.empty() && !balls.back()->is_moving)
    {
        balls.back()->is_moving = true;
        power_up->catchTime = 0.f;
    }
    // WHen Laser power up is active
    else if (power_up->active_type == 2)
    {
        power_up->shootLaser(player->getPosition());
    }    
}

void GameplayState::breakingOutAnimation()
{
    // Animation of moving through break into next level
    player->is_breaking_out = true;
    increaseScore(10000);
    for (auto& pilka : balls)
        pilka->is_moving = false;
    power_up->is_moving = false;
}

void GameplayState::increaseScore(int score)
{
    data->upper_interface.increaseScore(score);

    // Grants HP after getting 20000 score points
    if(bonus_hp_count == 0 && data->upper_interface.getScore() >= 20000)
    {
        if (hp < 6) 
            hp++;
        bonus_hp_count++;
        hp_sound.play();
    }
    //Grants HP after getting 60000 score points and then for each additonal 60000
    else if(bonus_hp_count > 0 && data->upper_interface.getScore() >= 60000 * bonus_hp_count)
    {
        if (hp < 6) 
            hp++;
        bonus_hp_count++;
        hp_sound.play();
    }
}

void GameplayState::handleHitSounds(std::pair<sf::Sound, sf::Clock>& sound)
{
    // One sound per 0.05 second
    if (sound.second.getElapsedTime().asSeconds() >= 0.05f)
    {
        sound.first.play();
        sound.second.restart();
    }
}

void GameplayState::checkCollisions()
{
    // Ball collision
    for (unsigned int k = 0; k < balls.size(); k++)
    {
        // with bricks
        for (unsigned int i = 0; i < bricks.size(); i++)
        {
            if (balls[k]->getGlobalBounds().intersects(bricks[i]->getGlobalBounds()))
            {
                Direction bounce_direction = bricks[i]->whichSide(balls[k]->getPosition(), balls[k]->velocity);
                
                if (bounce_direction != NoDirection)
                {
                    balls[k]->bounce(bounce_direction);
                    bricks[i]->handleHit();
                   
                    if (bricks[i]->type < 8)
                        handleHitSounds(brick_bounce_sound);
                    else
                        handleHitSounds(gold_brick_bounce_sound);

                    if (bricks[i]->hp < 1)
                    {
                        if (balls.size() == 1 && bricks[i]->type != 8)
                            //25% chance for dropping power up
                            if(random_number(data->generator) == 0)
                                power_up->drop(bricks[i]->getPosition());

                        balls[k]->increaseSpeed();
                        increaseScore(bricks[i]->point_value);

                        bricks.erase(bricks.begin() + i);
                    }   
                }
            }            
        }

        // with paddle
        if (balls[k]->getGlobalBounds().intersects(player->getGlobalBounds()))
        {
            int collision_side = player->getCollisionSide(*balls[k]);
            if (collision_side != 0)
            {
                balls[k]->bouncePaddle(collision_side);
                balls[k]->bounced = false;

                // when Catch is activ
                if (power_up->active_type == 1 && balls[k]->is_moving)
                {
                        player->catchBall(*balls[k]);
                        balls[k]->distane_from_paddle = balls[k]->getPosition().x - player->getPosition().x;
                        balls[k]->is_moving = false;                  
                }
            }
        }
        else if (!balls[k]->bounced && balls[k]->is_moving)
        {
            handleHitSounds(paddle_bounce_sound);
            balls[k]->bounced = true;
        }

        // with the bottom
        if (balls[k]->getPosition().y > HEIGHT + balls[k]->getGlobalBounds().height)
        {
            balls.erase(balls.begin() + k);
            if (balls.empty())
            {
                hp--;
                power_up->setPosition(0.f - power_up->getGlobalBounds().width, power_up->getPosition().y);
                power_up->active_type = 10;
                player->state = &player->death_animation;
                death_sound.play();
            }
        }
    }

    // Collision of power ups with the paddle
    if (player->getGlobalBounds().intersects(power_up->getGlobalBounds()))
        power_up->activatePowerUp(player, balls.back());

    // Collision of lasers with bricks
    for (unsigned int i = 0; i < power_up->lasers.size(); i++)
    {
        bool hit = false;
        for (int j = bricks.size() - 1; j >= 0; j--)
        {
            if (power_up->lasers[i]->getGlobalBounds().intersects(bricks[j]->getGlobalBounds()) && !power_up->lasers[i]->is_collision)
            {
                hit = true;
                bricks[j]->handleHit();
                if (bricks[j]->hp < 1)
                {
                    increaseScore(bricks[j]->point_value);
                    bricks.erase(bricks.begin() + j);
                } 
            }
        }
        if (hit)
            power_up->lasers[i]->is_collision = true;
                              
    }

    if (!Brick::removed_bricks.empty())
    {
        for (auto& brick : bricks)
            brick->checkNearbyBricks();
        Brick::removed_bricks.clear();
    }
}

void GameplayState::handlePowerUps(const float& dt)
{
    switch (power_up->active_type)
    {
    case 0:     // Slow
        for (auto& pilka : balls)
            pilka->decreaseSpeed(75.f);

        power_up->active_type = 10;
        break;

    case 1:     // Catch
        if (!balls.empty() && !balls.back()->is_moving)
        {
            balls.back()->setPosition(sf::Vector2f(player->getPosition().x + balls.back()->distane_from_paddle, balls.back()->getPosition().y));
            power_up->catchTime += dt;
            if (power_up->catchTime >= power_up->catchLimit) spacePressed();
        }            
        break;

    case 4:     // Disruption
        for (float i = -1; i < 1; i++)
        {
            balls.push_back(std::make_unique<Ball>(*balls.front()));
            balls.back()->is_moving = true;
            balls.back()->changeAngle(i * 5.f + balls.front()->degrees + (i + 1.f) * 5.f);
        }
        power_up->active_type = 10;
        break;

    case 5:     // Break
        power_up->is_break_out = true;
        power_up->active_type = 10;
        break;

    case 6:     // HP
        if (hp < 6) hp++;
        hp_sound.play();
        power_up->active_type = 10;
        break;

    default:
        break;
    }
}

void GameplayState::checkGameState(const float& dt)
{
        // If all bricks (except gold ones) were destroyed or if player died/broke out
    if (bricks.size() == Brick::gold_bricks_count || player->getGlobalBounds().left >= WIDTH)
    {
        if (wait_time == 0.f)
        {
            while (!balls.empty()) balls.pop_back();
            player->is_drawing = false;
            power_up->is_drawing = false;
        }

        wait_time += dt;
        if (wait_time > 1.f)
        {  
            // Game over
            if (hp == 0)
                data->state_machine.addState(StateRef(new TossCoin(level->number + 1, data)));
            // Respawn
            else if (player->death)
                level->start = true;
            // Next level
            else if (level->number < level->standard_level_count - 1)
                level->createLevel(bricks, background, hp_sprite);
            // Last level
            else if (level->number == level->standard_level_count - 1)
                data->state_machine.addState(StateRef(new BossState(data, hp)), true);

            reset();   
        }
    }
}

void GameplayState::reset()
{
    balls.push_back(std::make_unique<Ball>(data));
    player.reset();
    player = std::make_unique<Paddle>(data);
    power_up.reset();
    power_up = std::make_unique<PowerUp>(data);
    player->transformation.reset();
    wait_time = 0.f;
}

void GameplayState::saveGame()
{
    // If there is more than one ball, the speed of the slowest one is saved
    float velocity = 302.5f;
    for (auto& ball : balls)
        if (ball->speed < velocity)
            velocity = ball->speed;

    json gameplay_data = {
        {"hp", hp},
        {"score", data->upper_interface.getScore()},
        {"level", level->number},
        {"speed", velocity}
    };

    // Data concerning destroyed bricks
    json brickLayout;

    for (unsigned int i = 0; i < bricks.size(); i++)
        brickLayout.push_back({ bricks[i]->getPosition().x, bricks[i]->getPosition().y, bricks[i]->type, bricks[i]->hp });

    data->json_data[1] = gameplay_data;
    data->json_data[2] = brickLayout;

    std::ofstream file(DATA_FILE);
    file << data->json_data << std::endl;
}

void GameplayState::loadSave()
{
    // Reads data concerning: hp, score, level and ball speed
    hp = data->json_data[1]["hp"];
    if(data->json_data[1]["score"] != 0)
        data->upper_interface.increaseScore(data->json_data[1]["score"]);
    which_level = data->json_data[1]["level"];
    balls.back()->speed = data->json_data[1]["speed"];


    // Loads current level layout including destroyed bicks
    level->createLevel(bricks, background, hp_sprite, which_level, true);


    // Checks how much bonus HP was granted based on the score
    if (data->upper_interface.getScore() >= 20000 && data->upper_interface.getScore() < 60000)
        bonus_hp_count = 1;
    else if(data->upper_interface.getScore() >= 60000)
        bonus_hp_count = 1 + static_cast<int>(data->upper_interface.getScore() / 60000);

    // Removes current save
    data->json_data[1] = nullptr;
    data->json_data[2] = nullptr;

    // Adds cahnges to the data file (removed save)
    std::ofstream file(DATA_FILE);
    file << data->json_data;

    load_save = false;
}

void GameplayState::update(const float& dt)
{
    if (!menu->active)
    {
        if (level->start) 
            level->startLevel(player, balls.back(), dt);

        player->update(dt);

        for (auto& ball : balls) 
            ball->update(dt, sf::Vector2f(WIDTH, HEIGHT));

        checkCollisions();

        for (auto& brick : bricks)   
            brick->update(dt);

        power_up->update(dt);

        handlePowerUps(dt);

        checkGameState(dt);
    }
    else
        menu->update(dt);
 
    data->upper_interface.update(dt);

}

void GameplayState::draw(const float& dt)
{
    
    data->window.clear();

    data->window.draw(*background);

    for (auto& brick : bricks) 
        brick->draw();

    for (auto& ball : balls) 
        ball->drawShadow();

    power_up->draw();

    player->draw();

    for (auto& ball : balls) 
        ball->draw();

    for (int i = 0; i < hp - 1; i++)
        data->window.draw(*hp_sprite[i]);  

    level->draw();

    data->upper_interface.draw();

    menu->draw();

    data->window.display();
}
