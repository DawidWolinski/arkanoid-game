#pragma once
#include "../engine.h"
#include "../headers.h"
#include "../paddle_animation_states/paddle_spawn.h"
#include "../paddle_animation_states/paddle_transformation.h"
#include "../paddle_animation_states/paddle_death.h"
#include "../paddle_animation_states/paddle_standard.h"
#include "../paddle_animation_states/paddle_animation.h"

class Paddle : public sf::Sprite
{
public:

    //Konstruktor
    Paddle(GameDataRef data);

    //Zmienne
    bool is_drawing;
    bool is_moving;
    bool moving_right;
    bool is_breaking_out;
    bool death;
    int transition_type;
    const sf::Vector2f velocity;
      
    //Funkcje
    void update(const float& dt);
    void updateShadow();
    void draw();
    int getCollisionSide(const sf::Sprite& object);
    void catchBall(sf::Sprite& ball);

    //Animacje
    PaddleAnimation* state;
    PaddleSpawn spawn;
    PaddleStandard standard;
    PaddleTransformation transformation;
    PaddleDeath death_animation;

private:

    //Obiekty
    sf::Sprite shadow;

    //Zmienne
    GameDataRef data;
    float animation_time;

    //Funkcje
    void animation(const float& dt);
    void setTexture();
};

