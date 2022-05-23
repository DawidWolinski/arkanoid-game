#pragma once
#include "../engine.h"
#include "../headers.h"
#include "main_menu.h"

class GameOver : public State
{
public:

	//Konstruktor
	GameOver(GameDataRef data);

	//Funkcje
	virtual void initialise() override;
	virtual void handleEvents(const float& dt) override;
	virtual void update(const float& dt) override;
	virtual void draw(const float& dt) override;

private:

	GameDataRef data;

	//Zmienne
	sf::Sound game_over_sound;
	sf::Text game_over_text;
	float transition_time;
};

