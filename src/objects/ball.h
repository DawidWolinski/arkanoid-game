#pragma once
#include "../headers.h"
#include "../engine.h"
#include <math.h>

class Ball : public sf::Sprite
{
public:

    //Konstruktor
    Ball(GameDataRef data);

    //Zmienne
    bool is_moving;
    bool is_drawing;
    float distane_from_paddle;
    float degrees;
    float speed;
    sf::Vector2f velocity;
    bool bounced;

    //Funkcje
    void update(const float& dt, const sf::Vector2f& window_size);
    void draw();
    void drawShadow();
    void changeAngle(const float& degrees);
    void bounce(Direction bounce_direction, const float& degrees = 0);
    void bouncePaddle(const int& collision_side);
    void placeOnPaddle(const sf::Sprite& paddle);
    void increaseSpeed();
    void decreaseSpeed(float decreaseAmount = 50.f);



private:
    //Obiekty
    sf::Sprite shadow;

    //Zmienne
    GameDataRef data;
    Direction Right, Left, Down, Up;
    const float pi;
    std::uniform_int_distribution<int> random_number;
  
    //Funkcje
    void setTexture();
    void movement(const float& dt, const sf::Vector2f& window_size);
    
};

