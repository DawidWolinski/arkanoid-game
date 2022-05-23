#pragma once

#include "../engine.h"
#include "../headers.h"
#include "paddle.h"
#include "ball.h"


class Laser;

class PowerUp : public sf::Sprite
{
public:

	//Konstruktor
	PowerUp(GameDataRef data);

	//Zmienne
	bool is_drawing;
	bool is_moving;
	bool is_break_out;
	int type;
	int active_type;
	float catchTime;
	const float catchLimit;
	std::vector<std::unique_ptr<Laser>> lasers;

	//Funkcje
	void update(const float& dt);
	void draw();
	void drop(const sf::Vector2f& position);
	void activatePowerUp(std::unique_ptr<Paddle>& player, std::unique_ptr<Ball>& ball);
	void shootLaser(const sf::Vector2f& paddlePosition);

private:

	//Zmienne
	GameDataRef data;
	float animation_time;	
	float break_out_animation_time;	
	float laser_distance;
	sf::Sound enlarge_sound;
	sf::Sound laser_sound;
	sf::Sprite shadow;
	sf::Sprite break_out;
	const sf::Vector2f velocity;
	const sf::Vector2f laser_velocity;
	std::uniform_int_distribution<int> random_number;
	std::uniform_int_distribution<int> random_number_break_out;

	//Funkcje
	void setTexture();
	void animation(const float& dt);
	void updateLasers(const float& dt);
	void breakOutAnimation(const float& dt);
};


class Laser : public sf::Sprite
{
public:
	friend class PowerUp;

	//Konstruktor
	Laser(GameDataRef data, sf::Vector2f paddlePosition);

	//Zmienne
	bool is_collision;

private:

	//Zmienne
	GameDataRef data;
	bool remove;
	float animation_time;
	sf::Vector2f velocity;

	//Funkcje
	void update(const float& dt);
};
