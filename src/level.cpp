#include "level.h"

Level::Level(GameDataRef data)
    :number(-1), standard_level_count(32), draw_round_text(false), draw_ready_text(false), data(data)
{
    addBackgroundRects();
    setFonts();


    start_sound.setBuffer(data->asset_manager.getSound("start_sound"));
    start_sound.setVolume(25.f);

    boss_start_sound.setBuffer(data->asset_manager.getSound("boss_start_sound"));
    boss_start_sound.setVolume(25.f);
}

void Level::setFonts()
{
    round_text.setFont(data->asset_manager.getFont("font"));
    round_text.setString("ROUND 1");
    round_text.setCharacterSize(8 * static_cast<int>(data->scale));
    round_text.setScale(sf::Vector2f(1 / data->scale, 1 / data->scale));
    round_text.setOrigin(std::round(round_text.getLocalBounds().width / 2.f), std::round(round_text.getLocalBounds().height / 2.f));
    round_text.setFillColor(sf::Color::White);
    round_text.setPosition(sf::Vector2f(WIDTH/2.f, HEIGHT - 80.f));

    ready_text.setFont(*round_text.getFont());
    ready_text.setString("READY");
    ready_text.setCharacterSize(8 * static_cast<int>(data->scale));
    ready_text.setScale(sf::Vector2f(1 / data->scale, 1 / data->scale));
    ready_text.setOrigin(std::round(ready_text.getLocalBounds().width / 2.f), std::round(ready_text.getLocalBounds().height / 2.f));
    ready_text.setFillColor(sf::Color::White);
    ready_text.setPosition(sf::Vector2f(WIDTH/2.f, round_text.getPosition().y + 15.f));  
}

void Level::addBackgroundRects()
{
    int width = 224, height = 240;
    int x_distance = 8;
    int number_of_backgrounds = 5;

    for (int i = 0; i < number_of_backgrounds; i++)
        backgrounds.push_back(sf::IntRect(i * (width + x_distance), 0, width, height));
   

    // After 4 levels the background repeats
    int j = 0;
    for (unsigned int i = 0; i < standard_level_count; i++)
    {
        levels.push_back(backgrounds[j]);
        j++;
        if (j > 3) j = 0;
    }
    
    // Last level has separate background
    levels.push_back(backgrounds[4]);
}

void Level::draw()
{
    if (draw_round_text) data->window.draw(round_text);
    if (draw_ready_text) data->window.draw(ready_text);
}

// Standard level
void Level::createLevel(std::vector<std::unique_ptr<Brick>>& bricks, std::unique_ptr<sf::Sprite>& background, std::vector<std::unique_ptr<sf::Sprite>>& hp_sprite,int which_level, bool load_save)
{ 
    // First level number is 0

    if (which_level == 0 && number != -1)
        number++;
    else
        number = which_level;
        
    setBackground(background, hp_sprite);
    for (auto& object : hp_sprite) 
        object->setTexture(*background->getTexture());
    setLayout(bricks, load_save);
    round_text.setString("ROUND " + std::to_string(number + 1));
    round_text.setOrigin(std::round(round_text.getLocalBounds().width / 2.f), std::round(round_text.getLocalBounds().height / 2.f));
    start = true;
}

//BOSS
void Level::createLevel(std::unique_ptr<sf::Sprite>& background, std::unique_ptr<sf::Sprite>& boss_frame, std::vector<std::unique_ptr<sf::Sprite>>& hp_sprite)
{
    // First level number is 0

    number = levels.size() - 1;
   
    setBackground(background, hp_sprite);
    for (auto& object : hp_sprite) 
        object->setTexture(*background->getTexture());
    boss_frame->setTexture(data->asset_manager.getTexture("boss_texture"));
    boss_frame->setTextureRect(sf::IntRect(0, 0, 88, 135));
    boss_frame->setPosition(sf::Vector2f(72.f, 16.f+17.f));
    round_text.setString("ROUND " + std::to_string(number + 1));
    round_text.setOrigin(std::round(round_text.getLocalBounds().width / 2.f), std::round(round_text.getLocalBounds().height / 2.f));
    start = true;
}

void Level::setBackground(std::unique_ptr<sf::Sprite>& background, std::vector<std::unique_ptr<sf::Sprite>>& hp_sprite)
{
    if (background->getTexture() == NULL) 
        background->setTexture(data->asset_manager.getTexture("background_texture"));
    
    background->setTextureRect(levels[number]);
    background->setPosition(sf::Vector2f(0.f, 16.f));

    for (int i = 0; i < 5; i++)
    {
        if (hp_sprite.size() < 5)
            hp_sprite.push_back(std::make_unique<sf::Sprite>(data->asset_manager.getTexture("background_texture")));

        hp_sprite[i]->setTextureRect(sf::IntRect(levels[number].left + 8, levels[number].top + 240, 16, 8));
        hp_sprite[i]->setPosition(background->getPosition().x + 10.f + i * 16, background->getGlobalBounds().top + background->getGlobalBounds().height - hp_sprite[i]->getGlobalBounds().height);
    }
}

void Level::setLayout(std::vector<std::unique_ptr<Brick>>& bricks, bool load_save)
{
    bricks.clear();
    Brick::gold_bricks_count = 0;

    // Standard level creation based on a chosen layout 
    if (!load_save)
    {
        std::vector<int> layout = getLayout();
        if (layout.size() > 0 && layout.size() % 13 == 0)
        {
            int rows = layout.size() / 13;
            for (int i = 0; i < rows; i++)
                for (int j = 0; j < 13; j++)
                    if (layout[i * 13 + j] != 100)
                    {
                        bricks.push_back(std::make_unique<Brick>(layout[i * 13 + j], float(j) * 16.f + 16.f, float(i) * 8.f + 28.f, data, number + 1));
                        if (layout[i * 13 + j] == 9) Brick::gold_bricks_count++;
                    }
        }
    }
    // Creation of saved level (including destroyed bricks) based on json file
    else
    {
        Brick::gold_bricks_count = 0;

        for (unsigned int i = 0; i < data->json_data[2].size(); i++)
        {
            bricks.push_back(std::make_unique<Brick>(data->json_data[2][i][2], data->json_data[2][i][0], data->json_data[2][i][1], data, data->json_data[1]["level"], data->json_data[2][i][3]));
            if (bricks.back()->type == 9) Brick::gold_bricks_count++;
        }
    }

    for (auto& brick : bricks)
        brick->checkNearbyBricks(bricks);
}

void Level::startLevel(std::unique_ptr<Paddle>& player, std::unique_ptr<Ball>& balls, const float& dt)
{
    // Starting level sequence (also after death)

    switch (sequence_index)
    {
    case 0:                                     // Sound
        if (number < levels.size() - 1)
            start_sound.play();
        else
            boss_start_sound.play();
        sequence_index++;
        break;

    case 1:                                     // Round...      
        draw_round_text = true;
        animation_time += dt;
        if (animation_time > 1.5f) sequence_index++;
        break;

    case 2:                                     // Ready
        draw_ready_text = true;
        animation_time += dt;
        if (animation_time > 3.f) sequence_index++;
        break;

    case 3:                                     // Paddle spawn animation
        draw_round_text = false;
        draw_ready_text = false;
        player->is_drawing = true;
        player->state = &player->spawn;
        sequence_index++;
        break;

    case 4:                                     // Add ball
        if(player->state == &player->standard)
        {
            player->is_moving = true;
            balls->is_drawing = true;
            sequence_index++;
            animation_time = 0.f;
        }
        break;

    case 5:                                     // Wait
        balls->placeOnPaddle(*player);
        animation_time += dt;
        if (animation_time > 3.f) sequence_index++;
        break;

    case 6:                                     // Release ball
        balls->is_moving = true;
        start = false;
        animation_time = 0.f;
        sequence_index = 0;
        break;
    }

}

void Level::resetStart()
{
    start = false;
    sequence_index = 0;
    animation_time = 0.f;
}

std::vector<int> Level::getLayout()
{
    std::vector<int> level_layout;
    switch (number)
    {

        // 0 - white, 1 - orange, 2 - blue, 3 - green, 4 - red 
        // 5 - cyan, 6 - pink, 7 - yellow, 8 - silver, 9 - gold

    case 0:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 });
        level_layout.insert(level_layout.end(), { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 });
        level_layout.insert(level_layout.end(), { 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 });
        level_layout.insert(level_layout.end(), { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 });
        level_layout.insert(level_layout.end(), { 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 });
        level_layout.insert(level_layout.end(), { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 });

        break;
    case 1:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 0, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 0, 1, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 0, 1, 2, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 0, 1, 2, 3, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 0, 1, 2, 3, 4, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 0, 1, 2, 3, 4, 5, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 0, 1, 2, 3, 4, 5, 6, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 0, 1, 2, 3, 4, 5, 6, 7, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 0, 1, 2, 3, 4, 5, 6, 7, 0, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, _, _, _ });
        level_layout.insert(level_layout.end(), { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, _, _ });
        level_layout.insert(level_layout.end(), { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, _ });
        level_layout.insert(level_layout.end(), { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, _ });
        level_layout.insert(level_layout.end(), { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4 });
        break;

    case 2:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 0, 0, 0, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0, 0, 0 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 5, 5, 5, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 2, 2, 2 });
    
        break;

    case 3:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 1, 2, 3, 8, 5, _, 7, 0, 1, 2, 3, _ });
        level_layout.insert(level_layout.end(), { _, 2, 3, 8, 5, 6, _, 0, 1, 2, 3, 8, _ });
        level_layout.insert(level_layout.end(), { _, 3, 8, 5, 6, 7, _, 1, 2, 3, 8, 5, _ });
        level_layout.insert(level_layout.end(), { _, 8, 5, 6, 7, 0, _, 2, 3, 8, 5, 6, _ });
        level_layout.insert(level_layout.end(), { _, 5, 6, 7, 0, 1, _, 3, 8, 5, 6, 7, _ });
        level_layout.insert(level_layout.end(), { _, 6, 7, 0, 1, 2, _, 8, 5, 6, 7, 0, _ });
        level_layout.insert(level_layout.end(), { _, 7, 0, 1, 2, 3, _, 5, 6, 7, 0, 1, _ });
        level_layout.insert(level_layout.end(), { _, 0, 1, 2, 3, 8, _, 6, 7, 0, 1, 2, _ });
        level_layout.insert(level_layout.end(), { _, 1, 2, 3, 8, 5, _, 7, 0, 1, 2, 3, _ });
        level_layout.insert(level_layout.end(), { _, 2, 3, 8, 5, 6, _, 0, 1, 2, 3, 8, _ });
        level_layout.insert(level_layout.end(), { _, 3, 8, 5, 6, 7, _, 1, 2, 3, 8, 5, _ });
        level_layout.insert(level_layout.end(), { _, 8, 5, 6, 7, 0, _, 2, 3, 8, 5, 6, _ });
        level_layout.insert(level_layout.end(), { _, 5, 6, 7, 0, 1, _, 3, 8, 5, 6, 7, _ });
        level_layout.insert(level_layout.end(), { _, 6, 7, 0, 1, 2, _, 8, 5, 6, 7, 0, _ });


        break;

    case 4:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 7, _, _, _, _, _, 7, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 7, _, _, _, _, _, 7, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 7, _, _, _, 7, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 7, _, _, _, 7, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 8, 8, 8, 8, 8, 8, 8, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 8, 8, 8, 8, 8, 8, 8, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 8, 8, 4, 8, 8, 8, 4, 8, 8, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 8, 8, 4, 8, 8, 8, 4, 8, 8, _, _ });
        level_layout.insert(level_layout.end(), { _, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, 8, 8, 8, 8, 8, 8, 8, _, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, 8, _, _, _, _, _, 8, _, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, 8, _, _, _, _, _, 8, _, 8, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 8, 8, _, 8, 8, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 8, 8, _, 8, 8, _, _, _, _ });
        break;

    case 5:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 5, _, 4, _, 3, _, 2, _, 3, _, 4, _, 5 });
        level_layout.insert(level_layout.end(), { 5, _, 4, _, 3, _, 2, _, 3, _, 4, _, 5 });
        level_layout.insert(level_layout.end(), { 5, _, 4, _, 3, _, 2, _, 3, _, 4, _, 5 });
        level_layout.insert(level_layout.end(), { 5, _, 4, _, 3, _, 2, _, 3, _, 4, _, 5 });
        level_layout.insert(level_layout.end(), { 5, _, 4, _, 3, _, 2, _, 3, _, 4, _, 5 });
        level_layout.insert(level_layout.end(), { 5, _, 9, 1, 9, 1, 9, 1, 9, 1, 9, _, 5 });
        level_layout.insert(level_layout.end(), { 5, _, 4, _, 3, _, 2, _, 3, _, 4, _, 5 });
        level_layout.insert(level_layout.end(), { 5, _, 4, _, 3, _, 2, _, 3, _, 4, _, 5 });
        level_layout.insert(level_layout.end(), { 5, _, 4, _, 3, _, 2, _, 3, _, 4, _, 5 });
        level_layout.insert(level_layout.end(), { 5, _, 4, _, 3, _, 2, _, 3, _, 4, _, 5 });
        level_layout.insert(level_layout.end(), { 1, _, 1, _, 9, _, 1, _, 9, _, 1, _, 1 });
        level_layout.insert(level_layout.end(), { 5, _, 4, _, 3, _, 2, _, 3, _, 4, _, 5 });
        break;

    case 6:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, 7, 7, 6, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 7, 7, 6, 6, 5, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 7, 7, 6, 6, 5, 5, 4, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 7, 6, 6, 5, 5, 4, 4, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 7, 6, 6, 5, 5, 4, 4, 3, 3, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 6, 6, 5, 5, 4, 4, 3, 3, 2, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 6, 5, 5, 4, 4, 3, 3, 2, 2, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 5, 5, 4, 4, 3, 3, 2, 2, 1, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 5, 4, 4, 3, 3, 2, 2, 1, 1, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 4, 4, 3, 3, 2, 2, 1, 1, 0, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 3, 3, 2, 2, 1, 1, 0, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 3, 2, 2, 1, 1, 0, 0, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 2, 1, 1, 0, 0, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, 1, 0, 0, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });

        break;

    case 7:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 9, _, 9, _, 9, _, 9, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, _, _, _, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, 9, _, 9, _, _, _, 9, _, 9, 9, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, 0, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, 9, 1, 9, _, _, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 9, _, _, 2, _, _, 9, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, 3, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 9, _, _, 4, _, _, 9, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, 9, 5, 9, _, _, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, 6, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, 9, _, 9, _, _, _, 9, _, 9, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, _, _, _, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 9, _, 9, _, 9, _, 9, _, _, _ });

        break;

    case 8:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, 9, _, _, _, _, _, 9, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, 3, 9, _, _, _, _, _, 9, 3, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, 2, 9, _, _, _, _, _, 9, 2, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, 9, 9, _, _, _, _, _, 9, 9, 9, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 6, 0, 0, 0, 7, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 6, 1, 1, 1, 7, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 6, 2, 2, 2, 7, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 6, 3, 3, 3, 7, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 6, 4, 4, 4, 7, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 6, 5, 5, 5, 7, _, _, _, _ });

        break;

    case 9:
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, 5, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, 5, 2, 5, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, 5, 2, 0, 2, 5, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, 5, 2, 0, 2, 0, 2, 5, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, 5, 2, 0, 2, 8, 2, 0, 2, 5, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, 5, 2, 0, 2, 0, 2, 5, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, 5, 2, 0, 2, 5, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, 5, 2, 5, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, 5, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 });

        break;

    case 10:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, _, _, _, _, _, _, _, _, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, 8, 8, 8, 8, 8, 8, 8, _, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, 8, _, _, _, _, _, 8, _, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, 8, _, 8, 8, 8, _, 8, _, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, 8, _, 8, _, 8, _, 8, _, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, 8, _, 8, 8, 8, _, 8, _, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, 8, _, _, _, _, _, 8, _, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, 8, 8, 8, 8, 8, 8, 8, _, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, _, _, _, _, _, _, _, _, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, _ });

        break;

    case 11:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 });
        level_layout.insert(level_layout.end(), { _, _, _, _, 9, _, _, _, _, _, 9, 6, _ });
        level_layout.insert(level_layout.end(), { _, 9, 0, _, 9, _, _, _, _, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, 9, _, _, 9, _, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, 9, 3, _, 9, _, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, 9, _, _, 9, _, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, 1, 9, _, _, 9, _, 5, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, 9, _, _, 9, _, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, 9, _, _, 9, _, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, 9, _, 4, 9, _, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, 9, _, _, 9, _, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, 2, _, _, _, _, 9, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, 9, _, _, _, _, 7 });
        level_layout.insert(level_layout.end(), { _, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 });

        break;

    case 12:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 7, 7, 7, _, 0, 0, 0, _, 7, 7, 7, _ });
        level_layout.insert(level_layout.end(), { _, 6, 6, 6, _, 1, 1, 1, _, 6, 6, 6, _ });
        level_layout.insert(level_layout.end(), { _, 5, 5, 5, _, 2, 2, 2, _, 5, 5, 5, _ });
        level_layout.insert(level_layout.end(), { _, 4, 4, 4, _, 3, 3, 3, _, 4, 4, 4, _ });
        level_layout.insert(level_layout.end(), { _, 3, 3, 3, _, 4, 4, 4, _, 3, 3, 3, _ });
        level_layout.insert(level_layout.end(), { _, 2, 2, 2, _, 5, 5, 5, _, 2, 2, 2, _ });
        level_layout.insert(level_layout.end(), { _, 1, 1, 1, _, 6, 6, 6, _, 1, 1, 1, _ });
        level_layout.insert(level_layout.end(), { _, 0, 0, 0, _, 7, 7, 7, _, 0, 0, 0, _ });

        break;

    case 13:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 });
        level_layout.insert(level_layout.end(), { 9, _, _, _, _, _, _, _, _, _, _, _, 9 });
        level_layout.insert(level_layout.end(), { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 1, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 1 });
        level_layout.insert(level_layout.end(), { 9, _, _, _, _, _, _, _, _, _, _, _, 9 });
        level_layout.insert(level_layout.end(), { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2 });
        level_layout.insert(level_layout.end(), { 9, _, _, _, _, _, _, _, _, _, _, _, 9 });
        level_layout.insert(level_layout.end(), { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 });
        level_layout.insert(level_layout.end(), { 9, _, _, _, _, _, _, _, _, _, _, _, 9 });

        break;

    case 14:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 2, 0, 9, 2, 2, 2, 2, 2, 2, 2, 9, 0, 2 });
        level_layout.insert(level_layout.end(), { 2, 0, 7, 9, 2, 2, 2, 2, 2, 9, 3, 0, 2 });
        level_layout.insert(level_layout.end(), { 2, 0, 7, 7, 9, 2, 2, 2, 9, 3, 3, 0, 2 });
        level_layout.insert(level_layout.end(), { 2, 0, 7, 7, 7, 9, 0, 9, 3, 3, 3, 0, 2 });
        level_layout.insert(level_layout.end(), { 2, 0, 7, 7, 7, 7, 0, 3, 3, 3, 3, 0, 2 });
        level_layout.insert(level_layout.end(), { 2, 0, 7, 7, 7, 7, 0, 3, 3, 3, 3, 0, 2 });
        level_layout.insert(level_layout.end(), { 2, 0, 7, 7, 7, 7, 0, 3, 3, 3, 3, 0, 2 });
        level_layout.insert(level_layout.end(), { 2, 8, 7, 7, 7, 7, 0, 3, 3, 3, 3, 8, 2 });
        level_layout.insert(level_layout.end(), { 2, 2, 8, 7, 7, 7, 0, 3, 3, 3, 8, 2, 2 });
        level_layout.insert(level_layout.end(), { 2, 2, 2, 8, 7, 7, 0, 3, 3, 8, 2, 2, 2 });
        level_layout.insert(level_layout.end(), { 2, 2, 2, 2, 8, 7, 0, 3, 8, 2, 2, 2, 2 });
        level_layout.insert(level_layout.end(), { 2, 2, 2, 2, 2, 8, 0, 8, 2, 2, 2, 2, 2 });

        break;

    case 15:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, 9, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 0, 0, _, 0, 0, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 0, 0, _, _, 9, _, _, 0, 0, _, _ });
        level_layout.insert(level_layout.end(), { 0, 0, _, _, 1, 1, _, 1, 1, _, _, 0, 0 });
        level_layout.insert(level_layout.end(), { _, _, 1, 1, _, _, 9, _, _, 1, 1, _, _ });
        level_layout.insert(level_layout.end(), { 1, 1, _, _, 7, 7, _, 7, 7, _, _, 1, 1 });
        level_layout.insert(level_layout.end(), { _, _, 7, 7, _, _, 9, _, _, 7, 7, _, _ });
        level_layout.insert(level_layout.end(), { 7, 7, _, _, 3, 3, _, 3, 3, _, _, 7, 7 });
        level_layout.insert(level_layout.end(), { _, _, 3, 3, _, _, 9, _, _, 3, 3, _, _ });
        level_layout.insert(level_layout.end(), { 3, 3, _, _, 4, 4, _, 4, 4, _, _, 3, 3 });
        level_layout.insert(level_layout.end(), { _, _, 4, 4, _, _, 9, _, _, 4, 4, _, _ });
        level_layout.insert(level_layout.end(), { 4, 4, _, _, 5, 5, _, 5, 5, _, _, 4, 4 });
        level_layout.insert(level_layout.end(), { _, _, 5, 5, _, _, _, _, _, 5, 5, _, _ });
        level_layout.insert(level_layout.end(), { 5, 5, _, _, _, _, _, _, _, _, _, 5, 5 });

        break;

    case 16:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, 8, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 5, 5, 5, 8, 3, 3, 3, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 5, 5, 5, 0, 0, 0, 5, 5, 3, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 5, 5, 0, 0, 0, 0, 0, 5, 3, _, _ });
        level_layout.insert(level_layout.end(), { _, 5, 5, 5, 0, 0, 0, 0, 0, 5, 5, 3, _ });
        level_layout.insert(level_layout.end(), { _, 5, 5, 5, 0, 0, 0, 0, 0, 5, 5, 3, _ });
        level_layout.insert(level_layout.end(), { _, 5, 5, 5, 0, 0, 0, 0, 0, 5, 5, 3, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, _, 8, _, 8, _, 8, _, _, 8, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, 8, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, 8, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, 8, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 9, _, 9, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 9, 9, 9, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, 9, _, _, _, _, _, _, _ });

        break;

    case 17:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 1, _, 9, 7, 7, 7, 7, 7, 7, 7, 9, _, 1 });
        level_layout.insert(level_layout.end(), { 1, _, 9, 9, 7, 7, 7, 7, 7, 9, 9, _, 1 });
        level_layout.insert(level_layout.end(), { 1, _, 9, _, 9, 7, 7, 7, 9, _, 9, _, 1 });
        level_layout.insert(level_layout.end(), { 1, _, 9, _, 6, 9, 7, 9, 2, _, 9, _, 1 });
        level_layout.insert(level_layout.end(), { 1, _, 9, _, 6, _, 8, _, 2, _, 9, _, 1 });
        level_layout.insert(level_layout.end(), { 1, _, 9, _, 6, _, 3, _, 2, _, 9, _, 1 });
        level_layout.insert(level_layout.end(), { 1, _, 9, _, 6, _, 3, _, 2, _, 9, _, 1 });
        level_layout.insert(level_layout.end(), { 1, _, 9, _, 6, _, 3, _, 2, _, 9, _, 1 });
        level_layout.insert(level_layout.end(), { 1, _, 9, _, 6, _, 3, _, 2, _, 9, _, 1 });
        level_layout.insert(level_layout.end(), { 1, 9, 9, 9, 6, _, 3, _, 2, 9, 9, 9, 1 });

        break;

    case 18:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, 9, 9, 9, 9, 9, 9, 9, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 3, 4, 5, 6, 9, 6, 5, 4, 3, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 3, 4, 5, 6, 9, 6, 5, 4, 3, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 3, 4, 5, 6, 9, 6, 5, 4, 3, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 3, 4, 5, 6, 7, 6, 5, 4, 3, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 3, 4, 5, 6, 9, 6, 5, 4, 3, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 3, 4, 5, 6, 9, 6, 5, 4, 3, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 3, 4, 5, 6, 9, 6, 5, 4, 3, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, 9, 9, 9, 9, 9, 9, 9, 9, _, _ });


        break;

    case 19:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 9, 0, 9, 1, 9, 2, 9, 3, 9, 4, 9, 5, 9 });
        level_layout.insert(level_layout.end(), { 9, 6, 9, 8, 9, 8, 9, 8, 9, 8, 9, 7, 9 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 9, 6, 9, _, 9, _, 9, _, 9, _, 9, _, 9 });
        level_layout.insert(level_layout.end(), { 9, _, 9, 6, 9, _, 9, _, 9, _, 9, _, 9 });
        level_layout.insert(level_layout.end(), { 9, _, 9, _, 9, 6, 9, _, 9, _, 9, _, 9 });
        level_layout.insert(level_layout.end(), { 9, _, 9, _, 9, _, 9, 6, 9, _, 9, _, 9 });
        level_layout.insert(level_layout.end(), { 9, _, 9, _, 9, _, 9, _, 9, 6, 9, _, 9 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, 6, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, _, 9, _, 9, _, 9, 6, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, _, 9, _, 9, 6, 9, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, _, 9, 6, 9, _, 9, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 6, 9, _, 9, _, 9, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 6, _, _, _, _, 9, _, _, _, _, _, _ });

        break;

    case 20:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, _, _, _, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, 9, 9, 9, 9, 9, 9, 9, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, 9, _, _, _, _, _, 9, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, 9, _, _, _, _, _, 9, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, 9, _, 4, 4, 4, _, 9, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, 9, _, 3, 3, 3, _, 9, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, 9, _, 5, 5, 5, _, 9, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, 9, _, 0, 0, 0, _, 9, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, 9, _, _, _, _, _, 9, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, 9, 2, 2, 2, 2, 2, 9, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, _, _, _, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, _, _, _, _, 9, _ });
        level_layout.insert(level_layout.end(), { _, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, _ });

        break;

    case 21:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 });
        level_layout.insert(level_layout.end(), { 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 4, 4, 9, _, 9, 4, 4, 4, 9, _, 9, 4, 4 });
        level_layout.insert(level_layout.end(), { 4, 4, 9, _, 9, 4, 4, 4, 9, _, 9, 4, 4 });
        level_layout.insert(level_layout.end(), { 4, 4, 9, _, 9, 4, 4, 4, 9, _, 9, 4, 4 });
        level_layout.insert(level_layout.end(), { 4, 4, 9, _, 9, 4, 4, 4, 9, _, 9, 4, 4 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
        level_layout.insert(level_layout.end(), { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });

        break;

    case 22:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 8, 8, 8, _, 8, 8, 8, _, 8, 8, 8 });
        level_layout.insert(level_layout.end(), { _, _, 8, 3, 8, _, 8, 3, 8, _, 8, 3, 8 });
        level_layout.insert(level_layout.end(), { _, _, 8, 8, 8, _, 8, 8, 8, _, 8, 8, 8 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 8, 8, 8, _, 8, 8, 8, _, 8, 8, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, 4, 8, _, 8, 4, 8, _, 8, 4, 8, _ });
        level_layout.insert(level_layout.end(), { _, 8, 8, 8, _, 8, 8, 8, _, 8, 8, 8, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 8, 8, 8, _, 8, 8, 8, _, 8, 8, 8, _, _ });
        level_layout.insert(level_layout.end(), { 8, 5, 8, _, 8, 5, 8, _, 8, 5, 8, _, _ });
        level_layout.insert(level_layout.end(), { 8, 8, 8, _, 8, 8, 8, _, 8, 8, 8, _, _ });

        break;

    case 23:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, 0, 0, 0, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, 0, 0, 0, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, 0, 0, 0, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 0, 0, 0, 0, 0, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 0, 5, 0, 5, 0, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 0, 5, 5, 0, 5, 5, 0, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 5, 5, 5, 5, 5, 5, 5, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 5, 5, 5, 5, 5, 5, 5, 5, 5, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 5, 5, 5, 5, 5, 5, 5, 5, 5, _, _ });
        level_layout.insert(level_layout.end(), { _, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, _ });
        level_layout.insert(level_layout.end(), { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 });

        break;

    case 24:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 });
        level_layout.insert(level_layout.end(), { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 });
        level_layout.insert(level_layout.end(), { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 });
        level_layout.insert(level_layout.end(), { 9, 9, 9, 9, 9, 8, 8, 8, 9, 9, 9, 9, 9 });
        level_layout.insert(level_layout.end(), { 9, 4, 4, 4, 9, _, _, _, 9, 5, 5, 5, 9 });
        level_layout.insert(level_layout.end(), { 9, 4, 4, 4, 9, _, _, _, 9, 5, 5, 5, 9 });
        level_layout.insert(level_layout.end(), { 9, _, _, _, _, _, _, _, _, _, _, _, 9 });
        level_layout.insert(level_layout.end(), { 9, _, _, _, _, _, _, _, _, _, _, _, 9 });
        level_layout.insert(level_layout.end(), { 9, _, _, _, 9, 3, 3, 3, 9, _, _, _, 9 });
        level_layout.insert(level_layout.end(), { 9, _, _, _, 9, 3, 3, 3, 9, _, _, _, 9 });
        level_layout.insert(level_layout.end(), { 9, 8, 8, 8, 9, 9, 9, 9, 9, 8, 8, 8, 9 });

        break;

    case 25:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, 8, 8, 8, 9, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, 9, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 9, _, _, 2, 2, 2, _, _, 9, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 9, _, 3, 3, 3, 3, 3, _, 9, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 9, _, 5, 5, 5, 5, 5, _, 9, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 9, _, _, 6, 6, 6, _, _, 9, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, 9, _, _, _, _, _, 9, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, 9, 9, 9, 9, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });

        break;

    case 26:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 });
        level_layout.insert(level_layout.end(), { 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 });
        level_layout.insert(level_layout.end(), { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 });
        level_layout.insert(level_layout.end(), { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 });
        level_layout.insert(level_layout.end(), { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 });

        break;

    case 27:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 });
        level_layout.insert(level_layout.end(), { 2, 9, 9, 9, 9, 6, 9, 6, 9, 9, 9, 9, 2 });
        level_layout.insert(level_layout.end(), { 2, 9, _, _, _, _, _, _, _, _, _, 9, 2 });
        level_layout.insert(level_layout.end(), { 2, 9, 6, _, _, _, _, _, _, _, 6, 9, 2 });
        level_layout.insert(level_layout.end(), { 2, 9, 6, 6, _, _, _, _, _, 6, 6, 9, 2 });
        level_layout.insert(level_layout.end(), { 2, 9, 6, 6, 6, _, _, _, 6, 6, 6, 9, 2 });
        level_layout.insert(level_layout.end(), { _, 2, 9, 6, 6, 6, _, 6, 6, 6, 9, 2, _ });
        level_layout.insert(level_layout.end(), { _, _, 2, 9, 6, 6, 6, 6, 6, 9, 2, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, 2, 9, 6, 6, 6, 9, 2, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, 2, 9, 6, 9, 2, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, 2, 6, 2, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, 2, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });

        break;

    case 28:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 7, 7, 7, 7, 7, 9, _, 9, 7, 7, 7, 7, 7 });
        level_layout.insert(level_layout.end(), { 6, 6, 6, 6, 6, 9, _, 9, 6, 6, 6, 6, 6 });
        level_layout.insert(level_layout.end(), { 9, 9, 0, 9, 9, 9, _, 9, 9, 9, 0, 9, 9 });
        level_layout.insert(level_layout.end(), { 5, 5, 5, 5, 5, 9, _, 9, 5, 5, 5, 5, 5 });
        level_layout.insert(level_layout.end(), { 4, 4, 4, 4, 4, 9, _, 9, 4, 4, 4, 4, 4 });
        level_layout.insert(level_layout.end(), { 3, 3, 3, 3, 3, 9, _, 9, 3, 3, 3, 3, 3 });
        level_layout.insert(level_layout.end(), { 9, 9, 0, 9, 9, 9, _, 9, 9, 9, 0, 9, 9 });
        level_layout.insert(level_layout.end(), { 2, 2, 2, 2, 2, 9, _, 9, 2, 2, 2, 2, 2 });
        level_layout.insert(level_layout.end(), { 1, 1, 1, 1, 1, 9, _, 9, 1, 1, 1, 1, 1 });
        level_layout.insert(level_layout.end(), { 0, 0, 0, 0, 0, 9, _, 9, 0, 0, 0, 0, 0 });

        break;

    case 29:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 7, 6, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 7, 6, 5, 4, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 7, 6, 5, 4, 3, 2, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 7, 6, 5, 4, 3, 2, 1, 0, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, _, _, _ });
        level_layout.insert(level_layout.end(), { 8, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, _ });
        level_layout.insert(level_layout.end(), { _, 9, 8, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3 });
        level_layout.insert(level_layout.end(), { _, _, _, 9, 8, 2, 1, 0, 7, 6, 5, 4, 3 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, 9, 8, 0, 7, 6, 5, 4, 3 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, 9, 8, 6, 5, 4, 3 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, 9, 8, 4, 3 });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, 9, 8 });

        break;

    case 30:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { 3, _, 4, _, 5, _, 6, _, 7, _, 0, _, 1 });
        level_layout.insert(level_layout.end(), { 8, _, 8, _, 8, _, 8, _, 8, _, 8, _, 8 });
        level_layout.insert(level_layout.end(), { _, 5, _, 4, _, 3, _, 2, _, 1, _, 0, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, 8, _, 8, _, 8, _, 8, _, 8, _ });
        level_layout.insert(level_layout.end(), { 2, _, 3, _, 4, _, 5, _, 6, _, 7, _, 0 });
        level_layout.insert(level_layout.end(), { 8, _, 8, _, 8, _, 8, _, 8, _, 8, _, 8 });
        level_layout.insert(level_layout.end(), { _, 6, _, 5, _, 4, _, 3, _, 2, _, 1, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, 8, _, 8, _, 8, _, 8, _, 8, _ });
        level_layout.insert(level_layout.end(), { 1, _, 2, _, 3, _, 4, _, 5, _, 6, _, 7 });
        level_layout.insert(level_layout.end(), { 8, _, 8, _, 8, _, 8, _, 8, _, 8, _, 8 });
        level_layout.insert(level_layout.end(), { _, 7, _, 6, _, 5, _, 4, _, 3, _, 2, _ });
        level_layout.insert(level_layout.end(), { _, 8, _, 8, _, 8, _, 8, _, 8, _, 8, _ });
        level_layout.insert(level_layout.end(), { 0, _, 1, _, 2, _, 3, _, 4, _, 5, _, 6 });
        level_layout.insert(level_layout.end(), { 8, _, 8, _, 8, _, 8, _, 8, _, 8, _, 8 });

        break;

    case 31:
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, _, _, _, _, _, _, _, _, _, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, _, 9, _, 9, _, 9, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, _, 9, _, 9, _, 9, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, _, 9, _, 9, _, 9, 3, 3, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, _, 9, _, 9, _, 9, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, _, 9, _, 9, 4, 4, 4, 4, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, _, 9, _, 9, _, 9, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, _, 9, 5, 5, 5, 5, 5, 5, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, _, 9, _, 9, _, 9, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, 6, 6, 6, 6, 6, 6, 6, 6, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 9, _, 9, _, 9, _, 9, _, 9, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 7, 7, 7, 7, 7, 7, 7, 7, 7, _, _ });
        level_layout.insert(level_layout.end(), { _, _, 8, 8, 8, 8, 8, 8, 8, 8, 8, _, _ });

        break;
    }


    return level_layout;
}