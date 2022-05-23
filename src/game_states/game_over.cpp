#include "game_over.h"

GameOver::GameOver(GameDataRef data)
	: transition_time(0.f), data(data)
{

}

void GameOver::initialise()
{
    game_over_text.setFont(data->asset_manager.getFont("font"));
    game_over_text.setCharacterSize(8 * static_cast<int>(data->scale));
    game_over_text.setString("game over");
    game_over_text.setScale(sf::Vector2f(1 / data->scale, 1 / data->scale));
    game_over_text.setOrigin(sf::Vector2f(std::round(game_over_text.getLocalBounds().width / 2.f), std::round(game_over_text.getLocalBounds().height / 2.f)));
    game_over_text.setFillColor(sf::Color::White);
    game_over_text.setPosition(sf::Vector2f(WIDTH / 2.f, HEIGHT - 80.f));


	data->asset_manager.loadSound("dzwiek_koniec_gry", GAME_OVER_SOUND_FILE);
	game_over_sound.setBuffer(data->asset_manager.getSound("dzwiek_koniec_gry"));
	game_over_sound.setVolume(25.f);

	game_over_sound.play();
}

void GameOver::handleEvents(const float& dt)
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
			if (data->event.key.code == sf::Keyboard::Escape)
				data->window.close();
			if (data->event.key.code == sf::Keyboard::P || data->event.key.code == sf::Keyboard::Enter || data->event.key.code == sf::Keyboard::Space)
				transition_time += 5.f;
			break;
		}
	}
}

void GameOver::update(const float& dt)
{
	data->upper_interface.update(dt);

	transition_time += dt;
	
	if (transition_time >= 5.f)
	{
		data->upper_interface.reset();
		data->state_machine.addState(StateRef(new MenuState(data)));
	}
}

void GameOver::draw(const float& dt)
{
    data->window.clear();

    data->window.draw(game_over_text);
	data->upper_interface.draw();

    data->window.display();
}