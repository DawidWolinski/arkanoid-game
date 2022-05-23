#pragma once
#include "engine.h"
#include "headers.h"

class GameplayMenu
{
public:

	//Konstruktor
	GameplayMenu(GameDataRef data, bool boss = false);
	
	//Funkcje
	void update(const float& dt);
	void draw();
	void grayOutOption();
	void changeOption(int direction);

	//Zmienne
	int option_index;
	bool move_up;
	bool active;

private:

	GameDataRef data;

	//Funkcje
	void animateText(const float& dt);	
	void setCover();
	void setTexts();
	void setText(sf::Text& text, std::string text_string, float x, float y, unsigned int size, sf::Color color);

	//Zmienne
	bool boss;
	float animation_time;
	
	sf::RectangleShape cover;

	//Napisy
	std::vector <std::unique_ptr<sf::Text>> options;
	std::vector <std::unique_ptr<sf::Text>> information;	
};
