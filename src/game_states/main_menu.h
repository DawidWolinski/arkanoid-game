#pragma once

#include "../engine.h"
#include "../headers.h"
#include "scoreboard.h"

class MenuState : public State
{
public:

	//Konstruktor
	MenuState(GameDataRef data, bool start = false);

	//Funkcje
	void initialise() override;
	void handleEvents(const float& dt) override;
	void update(const float& dt) override;
	void draw(const float& dt) override;

private:
	
	GameDataRef data;
	sf::View view;

	//Zmienne
	Direction move_down, move_up;
	bool game_save;
	bool draw_taito;
	bool draw_options;
	unsigned int option_index;
	unsigned int sequence_index;
	float animation_time;
	float sequence_time;

	//Napisy
	std::vector <std::unique_ptr<sf::Text>> options;
	std::vector <std::unique_ptr<sf::Text>> taito;
	sf::Text all_rights;
	sf::Sprite game_logo;
	sf::Sprite copyright_symbol;

	//Funkcje
	void setFonts();
	void loadSounds();
	void loadTextures();
	void sequence(float dt);
	void setText(sf::Text& text, std::string text_string, float x, float y, unsigned int size = 8, sf::Color color = sf::Color::White, std::string font = "font");
	void animateText(const float& dt);
	void changeOption(Direction kierunek);
	void selectOption();
	void updateText();	
};

