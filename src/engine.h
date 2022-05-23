#pragma once

#include "state_machine.h"
#include "asset_manager.h"
#include "upper_interface.h"
#include <random>
#include <fstream>
#include <memory>

struct GameData
{
    GameData()
        : scale(3.f), event()
    {
        std::random_device rd;
        generator.seed(rd());
    }

    StateMachine state_machine;
    AssetManager asset_manager;
    sf::RenderWindow window;
    sf::Event event;
    json json_data;
    std::mt19937 generator;
    float scale;
    UpperInterface upper_interface;
};

typedef std::shared_ptr<GameData> GameDataRef;

class Silnik
{
public:
    Silnik(int window_width, int window_height, std::string window_title);

private:
    GameDataRef data = std::make_shared<GameData>();
    sf::Clock delta_clock;

    void run();
};



