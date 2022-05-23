#include "main_menu.h"
#include "gameplay.h"


MenuState::MenuState(GameDataRef data, bool start)
	:option_index(0), animation_time(0.f), sequence_index(0), draw_taito(false), sequence_time(0.f), move_down(Direction::Down), move_up(Direction::Up),
	data(data)
{
	// Check if intro should be displayed
	if (!start)
		draw_options = true;
	else
		draw_options = false;

	// Check if there is a game save
	if (data->json_data[1] == nullptr)
		game_save = false;
	else 
		game_save = true;
}


void MenuState::initialise()
{
	// View - increase size x3
	view.setSize(sf::Vector2f(WIDTH, HEIGHT));
	view.setCenter(sf::Vector2f(view.getSize().x / 2.f, view.getSize().y / 2.f));
	data->window.setView(view);

	data->scale = std::floor(data->window.getSize().x / WIDTH);
	
	setFonts();
	loadTextures();
	loadSounds();
}

void MenuState::loadTextures()
{
	data->asset_manager.loadTexture("arkanoid_ship_texture", ARKANOID_SHIP_TEXTURE_FILE);

	data->asset_manager.loadTexture("copyright_texture", COPYRIGHT_TEXTURE_FILE);

	data->asset_manager.loadTexture("background_texture", BACKGROUND_TEXTURE_FILE);
	data->asset_manager.loadTexture("brick_texture", BRICK_TEXTURE_FILE);
	data->asset_manager.loadTexture("paddle_texture", PADDLE_TEXTURE_FILE);
	data->asset_manager.loadTexture("bonus_texture", BONUS_TEXTURE_FILE);

	game_logo.setTexture(data->asset_manager.getTexture("arkanoid_ship_texture"));
	game_logo.setTextureRect(sf::IntRect(0, 0, 193, 42));
	game_logo.setOrigin(sf::Vector2f(game_logo.getGlobalBounds().width / 2.f, game_logo.getGlobalBounds().height / 2.f));
	game_logo.setPosition(sf::Vector2f(WIDTH / 2.f, 60.f));

	copyright_symbol.setTexture(data->asset_manager.getTexture("copyright_texture"));
	copyright_symbol.setTextureRect(sf::IntRect(3, 2, 18, 18));
	copyright_symbol.setOrigin(sf::Vector2f(copyright_symbol.getGlobalBounds().width / 2.f, copyright_symbol.getGlobalBounds().height / 2.f));
	copyright_symbol.setPosition(sf::Vector2f(taito[1]->getGlobalBounds().left - 8.f, taito[1]->getPosition().y + 4.5f));
	copyright_symbol.setScale(0.85f, 0.85f);
}

void MenuState::setFonts()
{
	data->asset_manager.loadFont("taito_font", TAITO_FONT_FILE);

	// Initial texts
	for (unsigned int i = 0; i < 3; i++)
		taito.push_back(std::make_unique<sf::Text>());

	setText(*taito[0], "TAITO", WIDTH / 2.f, HEIGHT - 90.f, 27, sf::Color::Red, "taito_font");
	setText(*taito[1], "1986 taito corp japan", WIDTH / 2.f, taito[0]->getPosition().y + taito[0]->getGlobalBounds().height / 2.f + 25.f);
	setText(*taito[2], "all rights reserved", WIDTH / 2.f, taito[1]->getPosition().y + taito[1]->getGlobalBounds().height / 2.f + 10.f);

	// Menu options
	for (unsigned int i = 0; i < 4; i++)
		options.push_back(std::make_unique<sf::Text>());
	
	setText(*options[0], "NEW GAME", WIDTH / 2.f, 125.f, 10);
	setText(*options[1], "CONTINUE GAME", WIDTH / 2.f, options[0]->getPosition().y + options[0]->getGlobalBounds().height + 20.f, 10);
	setText(*options[2], "HIGH SCORES", WIDTH / 2.f, options[1]->getPosition().y + options[1]->getGlobalBounds().height + 20.f, 10);
	setText(*options[3], "EXIT", WIDTH / 2.f, options[2]->getPosition().y + options[2]->getGlobalBounds().height + 20.f, 10);

	// Grays out "CONTINUE GAME" if there is no game save
	if (!game_save) 
		options[1]->setFillColor(sf::Color(75, 72, 72));
}

void MenuState::loadSounds()
{
	data->asset_manager.loadSound("start_sound", START_SOUND_FILE);
	data->asset_manager.loadSound("paddle_death_sound", PADDLE_DEATH_SOUND_FILE);
	data->asset_manager.loadSound("paddle_bounce_sound", PADDLE_BOUNCE_SOUND_FILE);
	data->asset_manager.loadSound("brick_bounce_sound", BRICK_BOUNCE_SOUND_FILE);
	data->asset_manager.loadSound("gold_brick_bounce_sound", GOLD_BRICK_BOUND_SOUND_FILE);
	data->asset_manager.loadSound("enlarge_sound", ENLARGE_SOUND_FILE);
	data->asset_manager.loadSound("hp_sound", HP_SOUND_FILE);
	data->asset_manager.loadSound("laser_sound", LASER_SOUND_FILE);
	data->asset_manager.loadSound("boss_start_sound", BOSS_START_SOUND_FILE);
}

void MenuState::sequence(float dt)
{
	// Sequence displayed after opening the game (company logo --> main_menu)

	sequence_time += dt;

	switch (sequence_index)
	{
	case 0:		// Company logo
		draw_taito = true;
		sequence_index++;
		break;
	case 1:		// Logo disappears after 2.5 seconds
		if (sequence_time >= 2.5f)
		{
			sequence_index++;
			draw_taito = false;
		}
		break;
	case 2:		// Main menu (options)
		if (sequence_time >= 4.f)
		{
			draw_options = true;
			sequence_time = 0.f;
			sequence_index = 0;
		}
		break;
	}
}

void MenuState::handleEvents(const float& dt)
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
			if (data->event.key.code == sf::Keyboard::S || data->event.key.code == sf::Keyboard::Down)
				if(draw_options)
					changeOption(move_down);
			if (data->event.key.code == sf::Keyboard::W || data->event.key.code == sf::Keyboard::Up)
				if (draw_options)
					changeOption(move_up);
			if (data->event.key.code == sf::Keyboard::Enter)
			{
				// Skips intro sequence and goes directly to options display
				if (!draw_options)
				{
					draw_taito = false;
					draw_options = true;
					sequence_index = 0;
				}
				else
					selectOption();
			}
			// Decreases screen resolution
			if (data->event.key.code == sf::Keyboard::K)
			{
				if (data->scale > 0.f)
				{
					if(data->scale > 1)
						data->scale--;
					data->window.setSize(sf::Vector2u(static_cast<int>(WIDTH) * static_cast<int>(data->scale), static_cast<int>(HEIGHT) * static_cast<int>(data->scale)));
					data->upper_interface.updateText(data->scale);
					updateText();
					data->window.setPosition(sf::Vector2i(data->window.getPosition().x + static_cast<int>(WIDTH / 2.f), data->window.getPosition().y + static_cast<int>(HEIGHT / 2.f)));
				}
				
			}
			// Increases screen resolution
			if (data->event.key.code == sf::Keyboard::L)
			{
				data->scale++;
				data->window.setSize(sf::Vector2u(static_cast<int>(WIDTH) * static_cast<int>(data->scale), static_cast<int>(HEIGHT) * static_cast<int>(data->scale)));
				data->upper_interface.updateText(data->scale);
				updateText();
				data->window.setPosition(sf::Vector2i(data->window.getPosition().x + static_cast<int>(WIDTH/2.f), data->window.getPosition().y + static_cast<int>(HEIGHT / 2.f)));
			}
            break;
        }
    }
}

void MenuState::animateText(const float& dt)
{
	// Animation of current option
	animation_time += dt;

	if (animation_time >= 0.5f)
	{
		animation_time = 0.f;

		if (options[option_index]->getFillColor().Yellow == options[option_index]->getFillColor()) options[option_index]->setFillColor(sf::Color::White);
		else options[option_index]->setFillColor(sf::Color::Yellow);	
	}
}

void MenuState::changeOption(Direction kierunek)
{
	if (kierunek == move_down)
	{
		if (option_index == options.size() - 1) option_index = 0;
		else option_index++;

		if (option_index == 1 && !game_save)
			option_index++;
	}
	else
	{
		if (option_index == 0) option_index = options.size() - 1;
		else option_index--;

		if (option_index == 1 && !game_save)
			option_index--;
	}

	// Removes highlight from previously animated option
	for (unsigned int i = 0; i < options.size(); i++)
	{
		if (i == 1 && game_save || i != option_index && i != 1)
			options[i]->setFillColor(sf::Color::White);
	}
		
	// Highlihts current option
	options[option_index]->setFillColor(sf::Color::Yellow);
	animation_time = 0.f;
}

void MenuState::selectOption()
{
	switch (option_index)
	{
	case 0:			// New game
		data->state_machine.addState(std::move(StateRef(new GameplayState(data, false))));
		//data->state_machine.addState(StateRef(new StanBoss(data, 5)), true);
		//data->state_machine.dodajStan(StateRef(new StanIntro(data)), true);
		break;
	case 1:			// Continue saved game
		data->state_machine.addState(StateRef(new GameplayState(data, true)));
		break;
	case 2:			// Scoreboard
		data->state_machine.addState(StateRef(new Scoreboard(0, 0, data, false)), false);
		break;
	case 3:			// Exit
		data->window.close();
		break;
	default:
		break;
	}
}

void MenuState::update(const float& dt)
{
	if (!draw_options) 
		sequence(dt);

	animateText(dt);

	data->upper_interface.update(dt);
}


void MenuState::draw(const float& dt)
{
	data->window.clear();

	if(draw_options) 
		for (auto& napis : options) 
			data->window.draw(*napis);
	else if (draw_taito)
	{
		for (auto& napis : taito)
			data->window.draw(*napis);

		data->window.draw(copyright_symbol);
	}
		
	data->window.draw(game_logo);

	data->upper_interface.draw();

	data->window.display();
}

void MenuState::setText(sf::Text& text, std::string text_string, float x, float y, unsigned int size, sf::Color color, std::string font)
{
	text.setFont(data->asset_manager.getFont(font));
	text.setCharacterSize(size * static_cast<int>(data->scale));
	text.setString(text_string);
	text.setScale(sf::Vector2f(1 / data->scale, 1 / data->scale));
	text.setOrigin(sf::Vector2f(std::round(text.getLocalBounds().width / 2.f), std::round(text.getLocalBounds().height / 2.f)));
	text.setFillColor(color);
	text.setPosition(sf::Vector2f(std::round(x), std::round(y)));
}

void MenuState::updateText()
{
	//Updates texts size after screen resolution was changed

	for (auto& napis : options)
	{
		napis->setCharacterSize(10 * static_cast<int>(data->scale));
		napis->setScale(sf::Vector2f(1 / data->scale, 1 / data->scale));
		napis->setOrigin(sf::Vector2f(std::round(napis->getLocalBounds().width / 2.f), std::round(napis->getLocalBounds().height / 2.f)));
	}

	/*
	taito[0]->setCharacterSize(27 * static_cast<int>(data->scale));
	taito[1]->setCharacterSize(8 * static_cast<int>(data->scale));
	taito[2]->setCharacterSize(8 * static_cast<int>(data->scale));

	for (auto& napis : taito)
	{
		napis->setScale(sf::Vector2f(1 / data->scale, 1 / data->scale));
		napis->setOrigin(sf::Vector2f(std::round(napis->getLocalBounds().width / 2.f), std::round(napis->getLocalBounds().height / 2.f)));
	}
	*/
}