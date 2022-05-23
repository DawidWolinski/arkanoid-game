#include "gameplay_menu.h"


GameplayMenu::GameplayMenu(GameDataRef data, bool boss)
	: boss(boss), active(false), animation_time(0.f), option_index(0), move_up(false), data(data)
{
	setTexts();
	setCover();
}

void GameplayMenu::setTexts()
{
	if (!boss)
	{
		for (int i = 0; i < 3; i++)
			options.push_back(std::make_unique<sf::Text>());

		setText(*options[0], "resume", WIDTH / 2.f, 100.f, 10, sf::Color::White);
		setText(*options[1], "save and exit", WIDTH / 2.f, options[0]->getPosition().y + options[0]->getGlobalBounds().height + 20.f, 10, sf::Color::White);
		setText(*options[2], "exit", WIDTH / 2.f, options[1]->getPosition().y + options[1]->getGlobalBounds().height + 20.f, 10, sf::Color::White);
	}
	else
	{
		for (int i = 0; i < 2; i++)
			options.push_back(std::make_unique<sf::Text>());

		setText(*options[0], "resume", WIDTH / 2.f, 115.f, 10, sf::Color::White);
		setText(*options[1], "exit", WIDTH / 2.f, options[0]->getPosition().y + options[0]->getGlobalBounds().height + 20.f, 10, sf::Color::White);
	}


	for (int i = 0; i < 2; i++)
		information.push_back(std::make_unique<sf::Text>());

	setText(*information[0], "progress can only be saved when", WIDTH / 2.f, 220.f, 4, sf::Color::White);
	setText(*information[1], "the ball flies upwards", WIDTH / 2.f, information[0]->getPosition().y + options[0]->getGlobalBounds().height / 2.f + 2.f, 4, sf::Color::White);
}

void GameplayMenu::setCover()
{
	cover.setSize(sf::Vector2f(WIDTH, HEIGHT));
	cover.setPosition(sf::Vector2f(0.f, 0.f));
	cover.setFillColor(sf::Color(0, 0, 0, 125));
}


void GameplayMenu::animateText(const float& dt)
{
	animation_time += dt;

	if (animation_time >= 0.5f)
	{
		animation_time = 0.f;

		if (options[option_index]->getFillColor().Yellow == options[option_index]->getFillColor()) 
			options[option_index]->setFillColor(sf::Color::White);
		else 
			options[option_index]->setFillColor(sf::Color::Yellow);
	}
}

void GameplayMenu::changeOption(int direction)
{
	if (direction == 0)
	{
		if (option_index == options.size() - 1) 
			option_index = 0;
		else 
			option_index++;

		if (option_index == 1 && !move_up && !boss)
			option_index++;
	}

	else
	{
		if (option_index == 0) 
			option_index = options.size() - 1;
		else 
			option_index--;

		if (option_index == 1 && !move_up && !boss)
			option_index--;
	}

	for (unsigned int i = 0; i < options.size(); i++)
	{
		if (i == 1 && move_up || i != option_index && i != 1 || boss && i != option_index)
			options[i]->setFillColor(sf::Color::White);
	}

	options[option_index]->setFillColor(sf::Color::Yellow);
	animation_time = 0.f;
}

void GameplayMenu::update(const float& dt)
{
	animateText(dt);
}

void GameplayMenu::draw()
{
	if (active)
	{
		data->window.draw(cover);

		for (auto& text : options)
			data->window.draw(*text);

		if(!move_up && !boss)
			for (auto& information_text : information)
				data->window.draw(*information_text);
	}	
}

void GameplayMenu::grayOutOption()
{
	if (move_up)
		options[1]->setFillColor(sf::Color::White);
	else
		options[1]->setFillColor(sf::Color(75, 72, 72));		
}

void GameplayMenu::setText(sf::Text& text, std::string text_string, float x, float y, unsigned int size, sf::Color color)
{
	text.setFont(data->asset_manager.getFont("font"));
	text.setCharacterSize(size * static_cast<int>(data->scale));
	text.setString(text_string);
	text.setScale(sf::Vector2f(1 / data->scale, 1 / data->scale));
	text.setOrigin(sf::Vector2f(std::round(text.getLocalBounds().width / 2.f), std::round(text.getLocalBounds().height / 2.f)));
	text.setFillColor(color);
	text.setPosition(sf::Vector2f(std::round(x), std::round(y)));
}