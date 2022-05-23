#pragma once

#include "../engine.h"
#include "../headers.h"
#include "../objects/paddle.h"
#include "../objects/ball.h"
#include "../objects/brick.h"
#include "../level.h"
#include "../objects/bonus.h"
#include "final_level.h"
#include "../gameplay_menu.h"
#include "toss_coin.h"

using json = nlohmann::json;

class GameplayState : public State
{
public:

	//Konstruktor
	GameplayState(GameDataRef data, bool load_save, const int& which_level = 0);

	//Funkcje
	void initialise() override;
	void handleEvents(const float& dt) override;
	void update(const float& dt) override;
	void draw(const float& dt) override;

private:

	GameDataRef data;
	Direction NoDirection;

	//Zmienne
	bool load_save;
	int hp;
	int bonus_hp_count;
	float wait_time;
	int which_level;
	std::uniform_int_distribution<int> random_number;

	//Funkcje
	void checkCollisions();
	void handlePowerUps(const float& dt);
	void checkGameState(const float& dt);
	void spacePressed();
	void reset();
	void saveGame();
	void loadSave();
	void setSounds();
	void increaseScore(int score);
	void breakingOutAnimation();
	void handleHitSounds(std::pair<sf::Sound, sf::Clock>& sound);

	//Dzwieki
	sf::Sound death_sound;
	sf::Sound hp_sound;
	std::pair<sf::Sound, sf::Clock> paddle_bounce_sound;
	std::pair<sf::Sound, sf::Clock> brick_bounce_sound;
	std::pair<sf::Sound, sf::Clock> gold_brick_bounce_sound;

	//Obiekty
	std::unique_ptr<Level> level;
	std::unique_ptr<Paddle> player;
	std::unique_ptr<sf::Sprite> background;
	std::vector <std::unique_ptr<Brick>> bricks;
	std::vector <std::unique_ptr<Ball>> balls;
	std::unique_ptr<PowerUp> power_up;
	std::vector<std::unique_ptr<sf::Sprite>> hp_sprite;
	std::unique_ptr<GameplayMenu> menu;
};