#pragma once

#include "../headers.h"
#include "../engine.h"
#include <array>


class Brick : public sf::Sprite
{
public:

    //Konstruktor i destruktor
    Brick(int type, float x, float y, GameDataRef data, int which_level = 1, int hp = 0);

    //Zmienne 
    int hp;
    int type;
    int point_value;
    static int gold_bricks_count;
    static std::vector<sf::Vector2f> removed_bricks;

    //Funkcje
    void draw();
    void update(const float& dt);
    void animation(const float& dt);
    void checkNearbyBricks(const std::vector<std::unique_ptr<Brick>>& bricks);
    void checkNearbyBricks();
    void handleHit();
    Direction whichSide(const sf::Vector2f& ball_position, const sf::Vector2f& velocity);

private:
    //Obiekty
    sf::Sprite shadow;

    //Zmienne
    GameDataRef data;
    Direction Right, Left, Down, Up, NoDirection;
    std::array<bool, 4> nearby_brick;
    float animation_time;
    bool is_animated;
   
    //Funkcje
    void setTexture(int which_level);
    void setShadow();
};

