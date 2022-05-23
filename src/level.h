#pragma once

#include <vector>
#include "objects/brick.h"
#include "engine.h"
#include "objects/ball.h"
#include "objects/paddle.h"

class Level
{
public:

    //Konstruktor
    Level(GameDataRef data);

    //Zmienne
    unsigned int number;     //Poziomy numerowane sa od 0 (wiec runda pierwsza to numer 0)
    unsigned int standard_level_count;
    bool start;
    std::vector <sf::IntRect> backgrounds;
    std::vector <sf::IntRect> levels;

    //Funkcje
    void createLevel(std::vector<std::unique_ptr<Brick>>& bricks, std::unique_ptr<sf::Sprite>& background, std::vector<std::unique_ptr<sf::Sprite>>& hp_sprite, int which_level = 0, bool load_save = false);
    void createLevel(std::unique_ptr<sf::Sprite>& background, std::unique_ptr<sf::Sprite>& boss_frame, std::vector<std::unique_ptr<sf::Sprite>>& hp_sprite);
    void startLevel(std::unique_ptr<Paddle>& player, std::unique_ptr<Ball>& balls, const float& dt);
    void resetStart();
    void draw();

private:

    //Zmienne
    GameDataRef data;
    enum NoBrick { _ = 100 };
    sf::Text round_text;
    sf::Text ready_text;
    float animation_time;
    int sequence_index;
    bool draw_round_text;
    bool draw_ready_text;

    //Dzwieki rozpoczynajace poziom
    sf::Sound start_sound;
    sf::Sound boss_start_sound;

    //Funkcje
    void setFonts();
    void addBackgroundRects();
    void setBackground(std::unique_ptr<sf::Sprite>& background, std::vector<std::unique_ptr<sf::Sprite>>& hp_sprite);
    void setLayout(std::vector<std::unique_ptr<Brick>>& bricks, bool load_save = false);
    std::vector<int> getLayout();

};

