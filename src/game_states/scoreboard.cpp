#include "scoreboard.h"

Scoreboard::Scoreboard(int which_level, int score, GameDataRef data, bool game_finished)
	:   which_level(which_level), score(score), data(data), game_finished(game_finished), place_number(999),
        new_score(true), highscores_limit(5), which_initial(0), letters{ "a", "", "" }, entered(false), 
        game_over(false), animation_time(0.f), transition_time(0.f)
{
    if (game_finished)
        round_text = "ALL";
    else
        round_text = std::to_string(which_level);
}

void Scoreboard::initialise()
{
    //resetScores(); //use this function instead of getScores() to reset scores
    setTexts();
    getScores();
    
}

void Scoreboard::handleEvents(const float& dt)
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
            if (data->event.key.code == sf::Keyboard::Escape && !new_score)
            {
                data->state_machine.removeState();
            }               
            if (data->event.key.code == sf::Keyboard::Space && !new_score)
            { 
                data->state_machine.removeState();
            }
            if (data->event.key.code == sf::Keyboard::Enter)
            {
                if (new_score)
                {
                    initials_texts[which_initial]->setFillColor(sf::Color::Yellow);

                    if (which_initial < 2)
                    {
                        which_initial++;
                        letters[which_initial] = "a";
                        name_text.setString(name_text.getString() + letters[which_initial]);
                    }
                    else if (!entered)
                        enterScore();
                }
                else
                    data->state_machine.removeState();
            }
            if (data->event.key.code == sf::Keyboard::BackSpace)
            {
                if (new_score)
                {
                    if (which_initial > 0)
                    {
                        letters[which_initial] = "";
                        which_initial--;

                        std::string temp = name_text.getString();
                        temp.erase(temp.length() - 1);
                        name_text.setString(temp);
                    }
                }
                else
                    data->state_machine.removeState();    
            }           
            updateInitials();
            break;
        case sf::Event::TextEntered:
            
                if (
                    (data->event.text.unicode >= (char)'0' && data->event.text.unicode <= (char)'9')
                    ||
                    (data->event.text.unicode >= (char)'a' && data->event.text.unicode <= (char)'z')
                    ||
                    (data->event.text.unicode >= (char)'A' && data->event.text.unicode <= (char)'Z')
                    ||
                    data->event.text.unicode == (char)' '
                    )
                {
                    letters[which_initial] = (char)data->event.text.unicode;

                    std::string temp = name_text.getString();
                    temp.erase(temp.length() - 1);
                    name_text.setString(temp);
                    name_text.setString(name_text.getString() + letters[which_initial]);        
                }
                updateInitials();
            break;
		}
	}
}

void Scoreboard::getScores()
{

    json current = {
        {"place", ""},
        {"score", score},
        {"round", round_text},
        {"name", ""}
    };

    // If there is no scores in the json file then it instantly goes to first place
    if (data->json_data[0] == nullptr)
    {
        current["place"] = "1ST";
        data->json_data[0].push_back(current);
        place_number = 0;
    }
    else
    {
        // Checks which place
        for (unsigned int i = 0; i < data->json_data[0].size(); i++)
        {
            if (score > data->json_data[0][i]["score"])
            {
                place_number = i;
                break;
            }
        }

        // If last place
        if (data->json_data[0].size() < highscores_limit)
            place_number = data->json_data[0].size();

        // Checks if any place was taken. If so, it adds a new score
        if (place_number == 999 || score == 0)
            new_score = false;
        else
            data->json_data[0].insert(data->json_data[0].begin() + place_number, current);
            
        // Removes last element if limit of highscores was exceeded
        if (data->json_data[0].size() > highscores_limit) data->json_data[0].erase(data->json_data[0].begin() + highscores_limit);
    }

    // Updates place numbers
    for (unsigned int i = 0; i < data->json_data[0].size(); i++)
        data->json_data[0][i]["place"] = getOrdinalSuffix(i + 1);

    // Creates scoreboard based on json file
    sf::Text text;
    std::string text_string;

    int i = 1;
    for (auto& first : data->json_data[0])
    {
        std::vector<sf::Text> temp;
        int a = 0;
        for (auto& second : first)
        {           
            if (a != 3)
                text_string = second;
            else
                text_string = std::to_string(static_cast<int>(second));

            setText(text, text_string, 0.f, header_two_texts[0]->getPosition().y + i * (header_two_texts[0]->getGlobalBounds().height + 10.f));
            temp.push_back(text);
            a++;
        }
        score_texts.push_back(temp);
        i++;
    }

    // Sets their horizontal position
    for (unsigned int a = 0; a < score_texts.size(); a++)
    {
        score_texts[a][0].setPosition(sf::Vector2f(header_two_texts[2]->getPosition().x, score_texts[a][0].getPosition().y));
        score_texts[a][1].setPosition(sf::Vector2f(39.f, score_texts[a][0].getPosition().y));
        score_texts[a][2].setPosition(sf::Vector2f(header_two_texts[1]->getPosition().x, score_texts[a][0].getPosition().y));
        score_texts[a][3].setPosition(sf::Vector2f(header_two_texts[0]->getPosition().x, score_texts[a][0].getPosition().y));
    }

    // Current score is highlighted in yellow
    if (new_score)
        for (auto& text : score_texts[place_number])
            text.setFillColor(sf::Color::Yellow);
}

std::string Scoreboard::getOrdinalSuffix(int number)
{
    std::string suffixes[] = { "ST", "ND", "RD", "TH", "TH", "TH", "TH", "TH", "TH", "TH" };

    if ((number % 100 >= 11 && number % 100 <= 13) || (number % 10 == 0))
        return std::to_string(number) + "TH";
    else
        return std::to_string(number) + suffixes[(number % 10) - 1];
}

void Scoreboard::enterScore()
{
    // Enters new score into json file

    entered = true;

    data->json_data[0][place_number]["name"] = name_text.getString();
    score_texts[place_number][0].setString(name_text.getString());
    score_texts[place_number][0].setOrigin(sf::Vector2f(score_texts[place_number][0].getLocalBounds().width, score_texts[place_number][0].getLocalBounds().height));        
 
    std::ofstream file(DATA_FILE);

    data->json_data[1] = nullptr;
    data->json_data[2] = nullptr;

    file << data->json_data << std::endl;
    
    game_over = true;
}

void Scoreboard::updateInitials()
{
    initials_texts[0]->setString(letters[0]);
    initials_texts[1]->setString(letters[1]);
    initials_texts[2]->setString(letters[2]);

    float space = name_text.getGlobalBounds().width - (initials_texts[0]->getGlobalBounds().width + initials_texts[1]->getGlobalBounds().width + initials_texts[2]->getGlobalBounds().width);
    
    if(name_text.getString().getSize() == 3) 
        space /= 2;

    initials_texts[1]->setPosition(sf::Vector2f(std::round(initials_texts[0]->getPosition().x + initials_texts[1]->getGlobalBounds().width + space), initials_texts[1]->getPosition().y));
    initials_texts[2]->setPosition(sf::Vector2f(std::round(initials_texts[1]->getPosition().x + initials_texts[2]->getGlobalBounds().width + space), initials_texts[2]->getPosition().y));   
}

void Scoreboard::animateInitial(const float& dt)
{
    animation_time += dt;

    if (animation_time >= 0.5f)
    {
        animation_time = 0.f;

        if (initials_texts[which_initial]->getFillColor().Red == initials_texts[which_initial]->getFillColor()) 
            initials_texts[which_initial]->setFillColor(sf::Color::White);
        else 
            initials_texts[which_initial]->setFillColor(sf::Color::Red);   
    }
}

void Scoreboard::animateText(const float& dt)
{
    animation_time += dt;

    if (animation_time >= 0.5f)
    {
        animation_time = 0.f;

        if (back_text.getFillColor().Yellow == back_text.getFillColor()) 
            back_text.setFillColor(sf::Color::White);
        else 
            back_text.setFillColor(sf::Color::Yellow);
    }
}

void Scoreboard::transition(const float& dt)
{
    transition_time += dt;

    if (transition_time >= 5.f)
        data->state_machine.addState(StateRef(new GameOver(data)));     
}

void Scoreboard::setTexts()
{
    setText(enter_initials_text, "enter your initials!", WIDTH / 2.f, 60.f, true, sf::Color::Red);

    for(unsigned int i = 0; i < 3; i++)
        header_one_texts.push_back(std::make_unique<sf::Text>());

    setText(*header_one_texts[0], "score", 80.f, 90.f, false, sf::Color::Yellow);
    setText(*header_one_texts[1], "round", header_one_texts[0]->getPosition().x + 50.f, 90.f, false, sf::Color::Yellow);
    setText(*header_one_texts[2], "name", header_one_texts[1]->getPosition().x + 45.f, 90.f, false, sf::Color::Yellow);


    // Top headers
    setText(score_text, std::to_string(score), header_one_texts[0]->getPosition().x, header_one_texts[0]->getPosition().y + 20.f);
    setText(round_one_text, round_text, header_one_texts[1]->getPosition().x, header_one_texts[1]->getPosition().y + 20.f, false, sf::Color::Yellow);
    setText(name_text, "aaa", header_one_texts[2]->getPosition().x, header_one_texts[2]->getPosition().y + 20.f, false, sf::Color::Yellow);
    name_text.setString("a");

    for (unsigned int i = 0; i < 3; i++)
        initials_texts.push_back(std::make_unique<sf::Text>(name_text));

    initials_texts[1]->setString("");
    initials_texts[2]->setString("");

    // Bottom headers (one is hidden)
    for (unsigned int i = 0; i < header_one_texts.size(); i++)
        header_two_texts.push_back(std::make_unique<sf::Text>(*header_one_texts[i]));

    header_two_texts[0]->setPosition(sf::Vector2f(104.f, 140.f));
    header_two_texts[1]->setPosition(sf::Vector2f(header_two_texts[0]->getPosition().x + header_two_texts[1]->getGlobalBounds().width + 15.f, 140.f));
    header_two_texts[2]->setPosition(sf::Vector2f(header_two_texts[1]->getPosition().x + header_two_texts[2]->getGlobalBounds().width + 15.f, 140.f));

    // Round text
    setText(round_two_text, "round " + std::to_string(which_level), WIDTH - 1.f, HEIGHT - 3.f);

    //Text if there is no new score
    for (unsigned int i = 0; i < 3; i++)
        no_new_score_texts.push_back(std::make_unique<sf::Text>());

    setText(*no_new_score_texts[0], "the following are", WIDTH / 2.f, 65.f, true, sf::Color::Yellow);
    setText(*no_new_score_texts[1], "the records of the bravest", WIDTH / 2.f, no_new_score_texts[0]->getPosition().y + no_new_score_texts[0]->getGlobalBounds().height/2.f + 10.f, true, sf::Color::Yellow);
    setText(*no_new_score_texts[2], "fighters of arkanoid", WIDTH / 2.f, no_new_score_texts[1]->getPosition().y + no_new_score_texts[1]->getGlobalBounds().height/2.f + 10.f, true, sf::Color::Yellow);

    // Back text
    setText(back_text, "back", WIDTH/2.f, HEIGHT - 10.f, true);
}

void Scoreboard::setText(sf::Text& text, std::string text_string, float x, float y, bool center, sf::Color color)
{
    text.setFont(data->asset_manager.getFont("font"));
    text.setCharacterSize(7 * static_cast<int>(data->scale));
    text.setString(text_string);
    text.setScale(sf::Vector2f(1 / data->scale, 1 / data->scale));
    if(center) 
        text.setOrigin(sf::Vector2f(std::round(text.getLocalBounds().width / 2.f), std::round(text.getLocalBounds().height / 2.f)));
    else 
        text.setOrigin(sf::Vector2f(text.getLocalBounds().width, text.getLocalBounds().height));
    text.setFillColor(color);
    text.setPosition(sf::Vector2f(std::round(x), std::round(y)));
}

void Scoreboard::resetScores()
{
    // Resets highscores in json file to default

    data->json_data = {
        {
            { {"name", "ssb"}, {"place", "1ST"}, {"round", "5"}, {"score", 50000} },
            { {"name", "snd"}, {"place", "2ND"}, {"round", "4"}, {"score", 45000} },
            { {"name", "tor"}, {"place", "3RD"}, {"round", "3"}, {"score", 40000} },
            { {"name", "onj"}, {"place", "4TH"}, {"round", "2"}, {"score", 35000} },
            { {"name", "gmd"}, {"place", "5TH"}, {"round", "1"}, {"score", 30000} }
        },
        nullptr, nullptr };

    std::ofstream file(DATA_FILE);
    file << data->json_data << std::endl;
}

void Scoreboard::update(const float& dt)
{

    if (new_score && !entered)
    {
        animateInitial(dt);
    }
    else
        animateText(dt);

    if (game_over)
    {
        transition(dt);
    }

    data->upper_interface.update(dt);

}

void Scoreboard::draw(const float& dt)
{
    data->window.clear();


    if (new_score)
    {
        data->window.draw(enter_initials_text);

        data->window.draw(score_text);

        data->window.draw(round_one_text);

        for (auto& inicjal : initials_texts)
            data->window.draw(*inicjal);

        for (auto& naglowek : header_one_texts)
            data->window.draw(*naglowek);

        data->window.draw(round_two_text);
    }
    else
    {
        for (auto& napis : no_new_score_texts)
            data->window.draw(*napis);

        data->window.draw(back_text);
    }

    for (auto& naglowek : header_two_texts)
        data->window.draw(*naglowek);

    for (auto& pierwszy : score_texts)
        for (auto& drugi : pierwszy)
            data->window.draw(drugi);

    data->upper_interface.draw();

    data->window.display();
}