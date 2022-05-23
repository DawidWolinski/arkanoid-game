#pragma once
#include "../engine.h"
#include "../headers.h"
#include "../game_states/game_over.h"

class TossCoin : public State
{
public:

	//Konstruktor
	TossCoin(int which_level, GameDataRef data);

	//Funckje
	virtual void initialise() override;
	virtual void handleEvents(const float& dt) override;
	virtual void update(const float& dt) override;
	virtual void draw(const float& dt) override;

private:

	GameDataRef data;

	//Zmienne
	int which_level;
	sf::Text to_continue;
	std::vector<std::unique_ptr<sf::Text>> instrukcja;
	std::vector<std::unique_ptr<sf::Text>> beforeTimer;
	sf::Text time_text;
	sf::Text credit;
	sf::Clock clock;
	float time;

	//Funkcje
	void setTexts();
	void setText(sf::Text& text, std::string text_string, float x, float y, bool center = false, sf::Color color = sf::Color::White);
	void aktualizujTimer();
};