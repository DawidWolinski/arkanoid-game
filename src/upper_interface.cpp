#include "upper_interface.h"

UpperInterface::UpperInterface()
    : score(0), is_drawing_up(true), animation_time(0.f), window(nullptr), font(nullptr), minimum_score(30000), highscore(50000)
{ 
}

void UpperInterface::initialise(sf::RenderWindow* window, sf::Font* font, int minimum_score, int highscore)
{
    this->window = window;
    this->font = font;

    this->minimum_score = minimum_score;
    this->highscore = highscore;

    setTexts();
}

void UpperInterface::setTexts()
{
    setText(highscore_header_text, "high score", WIDTH / 2.f, 6.f, sf::Color::Red);
    setText(highscore_text, std::to_string(highscore), WIDTH / 2.f, 14.f);
    setText(up_text, "1up", 37.f, highscore_header_text.getPosition().y, sf::Color::Red);
    setText(score_text, "00", 55.f, highscore_text.getPosition().y, sf::Color::White, false);
}

void UpperInterface::increaseScore(int score)
{
    this->score += score;
    score_text.setString(std::to_string(this->score));
    score_text.setOrigin(sf::Vector2f(score_text.getLocalBounds().width, score_text.getLocalBounds().height));

    if (this->score > highscore)
    {
        highscore = this->score;
        highscore_text.setString(std::to_string(highscore));
        highscore_text.setOrigin(sf::Vector2f(std::round(highscore_text.getLocalBounds().width / 2.f), highscore_text.getLocalBounds().height));
    }
}

bool UpperInterface::new_score()
{
    return score > minimum_score;
}

int UpperInterface::getScore()
{
    return score;
}

int UpperInterface::getHighscore()
{
    return highscore;
}

void UpperInterface::reset()
{
    std::ifstream file(DATA_FILE);
    json table;
    file >> table;

    highscore = table[0][0]["score"];
    minimum_score = table[0].back()["score"];

    highscore_text.setString(std::to_string(highscore));
    highscore_text.setOrigin(sf::Vector2f(std::round(highscore_text.getLocalBounds().width / 2.f), highscore_text.getLocalBounds().height));

    score = 0;
    score_text.setString("00");
    score_text.setOrigin(sf::Vector2f(score_text.getLocalBounds().width, score_text.getLocalBounds().height));
}

void UpperInterface::setText(sf::Text& text, std::string text_string, float x, float y, sf::Color color, bool center)
{
    text.setFont(*font);
    text.setCharacterSize(8*static_cast<int>(3));
    text.setString(text_string);
    text.setScale(sf::Vector2f(1 / 3.f, 1 / 3.f));

    if(center)
        text.setOrigin(sf::Vector2f(std::round(text.getLocalBounds().width / 2.f), text.getLocalBounds().height));
    else
        text.setOrigin(sf::Vector2f(text.getLocalBounds().width, text.getLocalBounds().height));

    text.setFillColor(color);
    text.setPosition(sf::Vector2f(std::round(x), std::round(y)));
}

void UpperInterface::updateText(float scale)
{
    highscore_header_text.setCharacterSize(8 * static_cast<int>(scale));
    highscore_header_text.setScale(sf::Vector2f(1 / scale, 1 / scale));
    highscore_header_text.setOrigin(sf::Vector2f(std::round(highscore_header_text.getLocalBounds().width / 2.f), highscore_header_text.getLocalBounds().height));

    highscore_text.setCharacterSize(8 * static_cast<int>(scale));
    highscore_text.setScale(sf::Vector2f(1 / scale, 1 / scale));
    highscore_text.setOrigin(sf::Vector2f(std::round(highscore_text.getLocalBounds().width / 2.f), highscore_text.getLocalBounds().height));

    up_text.setCharacterSize(8 * static_cast<int>(scale));
    up_text.setScale(sf::Vector2f(1 / scale, 1 / scale));
    up_text.setOrigin(sf::Vector2f(std::round(up_text.getLocalBounds().width / 2.f), up_text.getLocalBounds().height));

    score_text.setCharacterSize(8 * static_cast<int>(scale));
    score_text.setScale(sf::Vector2f(1 / scale, 1 / scale));
    score_text.setOrigin(sf::Vector2f(score_text.getLocalBounds().width, score_text.getLocalBounds().height));
}

void UpperInterface::update(const float& dt)
{
    animation_time += dt;

    if (animation_time >= 0.5f)
    {
        animation_time = 0.f;
        is_drawing_up = !is_drawing_up;
    }
}

void UpperInterface::draw()
{

    window->draw(highscore_header_text);

    window->draw(highscore_text);

    if (is_drawing_up)
        window->draw(up_text);

    window->draw(score_text);
}