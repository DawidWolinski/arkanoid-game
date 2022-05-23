#include "toss_coin.h"

TossCoin::TossCoin(int which_level, GameDataRef data)
	: which_level(which_level), data(data), time(0)
{
}

void TossCoin::initialise()
{
	setTexts();
}

void TossCoin::setTexts()
{
	for (unsigned int i = 0; i < 3; i++)
		instrukcja.push_back(std::make_unique<sf::Text>());

	for (unsigned int i = 0; i < 2; i++)
		beforeTimer.push_back(std::make_unique<sf::Text>());

	setText(to_continue, "to continue game", WIDTH / 2.f, 50.f, true, sf::Color::Red);

	setText(*instrukcja[0], "insert additional coin(s),", WIDTH / 2.f, to_continue.getPosition().y + to_continue.getGlobalBounds().height/2.f + 30.f, true);
	setText(*instrukcja[1], "hold fire button and push", instrukcja[0]->getGlobalBounds().left, instrukcja[0]->getPosition().y + instrukcja[0]->getGlobalBounds().height/2.f + 5.f);
	setText(*instrukcja[2], "1 or 2 player button", instrukcja[0]->getGlobalBounds().left, instrukcja[1]->getPosition().y + instrukcja[1]->getGlobalBounds().height + 5.f);

	setText(*beforeTimer[0], "before", instrukcja[0]->getGlobalBounds().left + 15.f, instrukcja[2]->getPosition().y + instrukcja[2]->getGlobalBounds().height + 25.f);
	setText(*beforeTimer[1], "timer reaches \"0\".", beforeTimer[0]->getGlobalBounds().left - 1.f, beforeTimer[0]->getPosition().y + beforeTimer[0]->getGlobalBounds().height + 5.f);

	
	setText(time_text, "time 9", beforeTimer[0]->getGlobalBounds().left + 35.f, beforeTimer[1]->getPosition().y + beforeTimer[1]->getGlobalBounds().height + 15.f, false, sf::Color::Yellow);
	setText(credit, "credit 0", WIDTH - 70.f, HEIGHT - 10.f);

}

void TossCoin::aktualizujTimer()
{
	// Counts how much time is left

	if (std::floor(clock.getElapsedTime().asSeconds()) > time)
	{
		time += 1.f;

		if (time == 9.f)
		{
			if (data->upper_interface.new_score())
				data->state_machine.addState(StateRef(new Scoreboard(which_level, data->upper_interface.getScore(), data)));
			else
				data->state_machine.addState(StateRef(new GameOver(data)));
		}		
		else
			time_text.setString("time " + std::to_string(static_cast<int>(9.f - time)));
	}
}

void TossCoin::handleEvents(const float& dt)
{
	while (data->window.pollEvent(data->event))
	{
		switch (data->event.type)
		{
		case sf::Event::Closed:
			data->window.close();
			break;
		case sf::Event::Resized:
			data->window.setPosition(sf::Vector2i(data->window.getPosition().x - int(WIDTH) / 2, data->window.getPosition().y - int(HEIGHT) / 2));
			break;
		case sf::Event::KeyPressed:
			if (data->event.key.code == sf::Keyboard::Enter || data->event.key.code == sf::Keyboard::Space || data->event.key.code == sf::Keyboard::Escape)
			{
				if (data->upper_interface.new_score())
					data->state_machine.addState(StateRef(new Scoreboard(which_level, data->upper_interface.getScore(), data)));
				else
					data->state_machine.addState(StateRef(new GameOver(data)));
			}
			break;
		}
	}
}

void TossCoin::update(const float& dt)
{
	data->upper_interface.update(dt);

	aktualizujTimer();
}

void TossCoin::draw(const float& dt)
{
	data->window.clear();

	data->upper_interface.draw();

	data->window.draw(to_continue);

	for(auto& test : instrukcja)
		data->window.draw(*test);

	for (auto& text : beforeTimer)
		data->window.draw(*text);

	data->window.draw(time_text);

	data->window.draw(credit);

	data->window.display();
}

void TossCoin::setText(sf::Text& text, std::string text_string, float x, float y, bool center, sf::Color color)
{
	text.setFont(data->asset_manager.getFont("font"));
	text.setCharacterSize(8 * static_cast<int>(data->scale));
	text.setString(text_string);
	text.setScale(sf::Vector2f(1 / data->scale, 1 / data->scale));
	if(center)
		text.setOrigin(sf::Vector2f(std::round(text.getLocalBounds().width / 2.f), std::round(text.getLocalBounds().height / 2.f)));
	text.setFillColor(color);
	text.setPosition(sf::Vector2f(std::round(x), std::round(y)));
}