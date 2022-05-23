#pragma once
#include "../engine.h"
#include "../headers.h"
#include "game_over.h"

using json = nlohmann::json;

class Scoreboard : public State
{
public:

	//Konstruktor
	Scoreboard(int which_level, int score, GameDataRef data, bool game_finished = false);

	//Funkcje
	virtual void initialise() override;
	virtual void handleEvents(const float& dt) override;
	virtual void update(const float& dt) override;
	virtual void draw(const float& dt) override;

private:

	GameDataRef data;

	//Zmienne
	int place_number;	
	int score;
	std::string round_text;
	int which_level;
	unsigned int highscores_limit;
	int which_initial;
	std::string letters[3];
	bool entered;
	bool game_finished;	
	bool new_score;
	bool game_over;
	float animation_time;
	float transition_time;
	
	//Napisy
	sf::Text round_one_text;
	sf::Text enter_initials_text;
	sf::Text name_text;
	sf::Text score_text;
	sf::Text round_two_text;
	sf::Text back_text;
	std::vector<std::unique_ptr<sf::Text>> no_new_score_texts;
	std::vector<std::unique_ptr<sf::Text>> header_one_texts;
	std::vector<std::unique_ptr<sf::Text>> header_two_texts;
	std::vector<std::unique_ptr<sf::Text>> initials_texts;
	std::vector<std::vector<sf::Text>> score_texts;

	//Funkcje
	void setTexts();
	void setText(sf::Text& text, std::string text_string, float x, float y, bool center = false, sf::Color color = sf::Color::White);
	void getScores();
	void resetScores();
	void enterScore();
	void updateInitials();
	void animateInitial(const float& dt);
	void animateText(const float& dt);
	void transition(const float& dt);
	std::string getOrdinalSuffix(int number);
};

