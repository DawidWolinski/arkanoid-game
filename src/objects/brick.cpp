#include "brick.h"

int Brick::gold_bricks_count;
std::vector<sf::Vector2f> Brick::removed_bricks;


Brick::Brick(int type, float x, float y, GameDataRef data, int which_level, int hp)
    :type(type), nearby_brick{}, is_animated(false), hp(hp), data(data),
    Down(Direction::Down), Up(Direction::Up), Left(Direction::Left), Right(Direction::Right), NoDirection(Direction::NoDirection)
{
    setTexture(which_level); 
    setOrigin(sf::Vector2f(getGlobalBounds().width / 2.f, getGlobalBounds().height / 2.f));
    setPosition(sf::Vector2f(x, y));

    setShadow();   
}

void Brick::setTexture(int which_level)
{
    sf::Sprite::setTexture(data->asset_manager.getTexture("brick_texture"));
    switch (type)
    {
    case 0:
        setTextureRect(sf::IntRect(0, 0, 16, 8));     ////    WHITE
        hp = 1;
        point_value = 50;
        break;
    case 1:
        setTextureRect(sf::IntRect(16, 0, 16, 8));    ////    ORANGE
        hp = 1;
        point_value = 60;
        break;
    case 2:
        setTextureRect(sf::IntRect(32, 0, 16, 8));     ////    CYAN
        hp = 1;
        point_value = 70;
        break;
    case 3:
        setTextureRect(sf::IntRect(48, 0, 16, 8));     ////    GREEN
        hp = 1;
        point_value = 80;
        break;
    case 4:
        setTextureRect(sf::IntRect(0, 8, 16, 8));     ////    RED
        hp = 1;
        point_value = 90;
        break;
    case 5:
        setTextureRect(sf::IntRect(16, 8, 16, 8));     ////    BLUE
        hp = 1;
        point_value = 100;
        break;
    case 6:
        setTextureRect(sf::IntRect(32, 8, 16, 8));     ////    PINK
        hp = 1;
        point_value = 110;
        break;
    case 7:
        setTextureRect(sf::IntRect(48, 8, 16, 8));     ////    YELLOW
        hp = 1;
        point_value = 120;
        break;
    case 8:
        setTextureRect(sf::IntRect(0, 16, 16, 8));     ////    SILVER
        if (hp == 0)
            hp = 2 + static_cast<int>(std::floor(which_level / 8));
        point_value = 50 * which_level;
        break;
    case 9:
        setTextureRect(sf::IntRect(0, 24, 16, 8));     ////    GOLD
        hp = 100;
        point_value = 0;
        break;
    default:
        setTextureRect(sf::IntRect(0, 0, 16, 8));
        hp = 1;
        point_value = 50;
    }  
}

void Brick::setShadow()
{
    shadow.setTexture(data->asset_manager.getTexture("brick_texture"));

    if (getPosition().x < 208) 
        shadow.setTextureRect(sf::IntRect(0, 0, 16, 8));
    //Cien klockow dotykajacych prawej sciany jest uciety
    else 
        shadow.setTextureRect(sf::IntRect(0, 0, 8, 8));

    shadow.setColor(sf::Color(0, 0, 0, 100));
    shadow.setPosition(sf::Vector2f(getPosition().x, getPosition().y + getGlobalBounds().height / 2.f));
}

void Brick::update(const float& dt)
{
    animation(dt);
}

void Brick::draw()
{
    data->window.draw(shadow);
    data->window.draw(*this);  
}

void Brick::checkNearbyBricks(const std::vector<std::unique_ptr<Brick>>& bricks)
{
    for (auto& klocek : bricks)
    { 
        if (getPosition().y == klocek->getPosition().y)
        {
            if (getPosition().x == klocek->getPosition().x - klocek->getGlobalBounds().width) 
                nearby_brick[static_cast<int>(Right)] = true;
            else if (getPosition().x == klocek->getPosition().x + klocek->getGlobalBounds().width) 
                nearby_brick[static_cast<int>(Left)] = true;
        }
        else if (getPosition().x == klocek->getPosition().x)
        {
            if (getPosition().y == klocek->getPosition().y - klocek->getGlobalBounds().height) 
                nearby_brick[static_cast<int>(Down)] = true;
            else if (getPosition().y == klocek->getPosition().y + klocek->getGlobalBounds().height) 
                nearby_brick[static_cast<int>(Up)] = true;
        }      
    }

    // Left and right walls of the map are also treated as nearby bricks
    if (getPosition().x - getGlobalBounds().width/2.f == 8.f)
        nearby_brick[static_cast<int>(Left)] = true;
    else if (getPosition().x + getGlobalBounds().width/2.f == 216.f)
        nearby_brick[static_cast<int>(Right)] = true;

}

void Brick::checkNearbyBricks()
{
    // Checks if destroyed brick was next to other brick
    for (auto& pozycja : removed_bricks)
    {
        if (getPosition().y == pozycja.y)
        {
            if (getPosition().x + getGlobalBounds().width == pozycja.x)
                nearby_brick[static_cast<int>(Right)] = false;
            else if (getPosition().x - getGlobalBounds().width == pozycja.x)
                nearby_brick[static_cast<int>(Left)] = false;
        }
        else if (getPosition().x == pozycja.x)
        {
            if (getPosition().y + getGlobalBounds().height == pozycja.y)
                nearby_brick[static_cast<int>(Down)] = false;
            else if (getPosition().y - getGlobalBounds().height == pozycja.y)
                nearby_brick[static_cast<int>(Up)] = false;
        }
    }
}

Direction Brick::whichSide(const sf::Vector2f& ball_position, const sf::Vector2f& velocity)
{
    // Checks from which side the brick was hit

    float dx = std::abs(ball_position.x - getPosition().x) - getGlobalBounds().width / 2.f;
    float dy = std::abs(ball_position.y - getPosition().y) - getGlobalBounds().height / 2.f;

        if (dy < dx)
        {
            if (ball_position.x > getPosition().x && !nearby_brick[static_cast<int>(Right)] && velocity.x < 0)
                return Right;
            else if (ball_position.x < getPosition().x && !nearby_brick[static_cast<int>(Left)] && velocity.x > 0)
                return Left;
        }
        else
        {
            if (ball_position.y > getPosition().y && !nearby_brick[static_cast<int>(Down)] && velocity.y < 0)
                return Down;
            else if (ball_position.y < getPosition().y && !nearby_brick[static_cast<int>(Up)] && velocity.y > 0)
                return Up;
        }  
    
    return NoDirection;
}

void Brick::handleHit()
{
    if (hp != 100) hp--;

    if (hp == 0) 
        removed_bricks.push_back(getPosition());
    else 
        is_animated = true;
}

void Brick::animation(const float& dt)
{
    // Hit animation for silver and gold bricks
    if (is_animated)
    {
        animation_time += 6.f * dt;
        if (animation_time > 0.5f)
        {
            if (getTextureRect().left == 80)
            {
                setTextureRect(sf::IntRect(0, getTextureRect().top, 16, 8));
                is_animated = false;
            }  
            else
                setTextureRect(sf::IntRect(getTextureRect().left + 16, getTextureRect().top, 16, 8));
            animation_time -= animation_time;
        }
    }
}