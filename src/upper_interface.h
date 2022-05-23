#pragma once
#include <SFML/Graphics.hpp>
#include "headers.h"
#include "../include/json.hpp"
#include <fstream>

using json = nlohmann::json;

class UpperInterface
{
public:

	//Konstruktor
	UpperInterface();
	
	//Zmienne
	bool new_score();
	int getScore();
	int getHighscore();

	//Funkcje
	void update(const float& dt);
	void draw();
	void initialise(sf::RenderWindow* window, sf::Font* font, int minimum_score, int highscore);
	void increaseScore(int score);
	void reset();
	void updateText(float skala);

private:

	//Zmienne
	int highscore;
	int score;
	int minimum_score;
	float animation_time;
	bool is_drawing_up;

	sf::RenderWindow* window;
	sf::Font* font;

	//Napisy
	sf::Text highscore_header_text;
	sf::Text up_text;
	sf::Text highscore_text;
	sf::Text score_text;

	//Funkcje
	void setTexts();
	void setText(sf::Text& text, std::string text_string, float x, float y, sf::Color color = sf::Color::White, bool center = true);
};

