#include "engine.h"
#include "headers.h"

int main()
{
    int scale = 3;
    if (sf::VideoMode::getDesktopMode().height < 864)
        scale = 2;

    Silnik silnik(scale * static_cast<int>(WIDTH), scale * static_cast<int>(HEIGHT), "Arkanoid");

    return 0;
}