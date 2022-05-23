#pragma once

#include "../engine.h"
#include "../gameplay_menu.h"
#include "../objects/paddle.h"
#include "../objects/ball.h"
#include "../level.h"
#include "../objects/boss.h"
#include "toss_coin.h"

class StanBoss : public State
{
public:

	//Konstruktor
	StanBoss(GameDataRef data, int hp);

	//Funkcje
	void initialise() override;
	void handleEvents(const float& dt) override;
	void update(const float& dt) override;
	void draw(const float& dt) override;

private:

	//Zmienne
	GameDataRef data;
	Direction NoDirection;

	int score;
	int hp;
	float wait_time;

	//Funkcje
	void loadSounds();
	void checkCollisions();
	void checkGameState(const float& dt);
	void reset();

	//Dzwieki
	sf::Sound death_sound;
	sf::Sound paddle_bounce_sound;
	sf::Sound boss_bounce_sound;
	//sf::Music dzwiekOutro;

	//Obiekty
	std::unique_ptr<Level> level;
	std::unique_ptr<Paddle> player;
	std::unique_ptr<sf::Sprite> background;
	std::unique_ptr<sf::Sprite> frame;
	std::unique_ptr<Boss> boss;
	std::vector <std::unique_ptr<Ball>> balls;
	std::vector<std::unique_ptr<sf::Sprite>> hp_sprite;
	std::unique_ptr<GameplayMenu> menu;
};
