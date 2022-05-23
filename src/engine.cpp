#include "engine.h"
#include "headers.h"
#include "game_states/main_menu.h"


Silnik::Silnik(int window_width, int window_height, std::string window_title)
{
	 
	data->window.create(sf::VideoMode(window_width, window_height), window_title, sf::Style::Close | sf::Style::Titlebar);
	data->window.setFramerateLimit(FPS_LIMIT);
	data->window.setPosition(sf::Vector2i(static_cast<int>(sf::VideoMode::getDesktopMode().width/2) - static_cast<int>(data->window.getSize().x/2), static_cast<int>(sf::VideoMode::getDesktopMode().height/2) - static_cast<int>(data->window.getSize().y/2) - 30));

	std::ifstream file(DATA_FILE);
	file >> data->json_data;

	data->asset_manager.loadFont("font", FONT_FILE);
	data->upper_interface.initialise(&data->window, &data->asset_manager.getFont("font"), data->json_data[0].back()["score"], data->json_data[0][0]["score"]);
	data->state_machine.addState(StateRef(new MenuState(data, true)));

	run();

	data->state_machine.removeAllStates();
}

void Silnik::run()
{
	while (data->window.isOpen())
	{
		float dt = delta_clock.restart().asSeconds();

		// Maximum dt to prevent objects from moving too far in case game freezes
		if(dt > 0.0175f)
			dt = 0.0175f;

		data->state_machine.handleStateChange();

		data->state_machine.getCurrentState()->handleEvents(dt);
		data->state_machine.getCurrentState()->update(dt);
		data->state_machine.getCurrentState()->draw(dt);
	}
}