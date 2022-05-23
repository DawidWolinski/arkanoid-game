#pragma once

#include <map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class AssetManager
{
public:
	//AssetManager();
	//~AssetManager();

	void loadTexture(std::string name, std::string file_name);
	sf::Texture& getTexture(std::string name);

	void loadFont(std::string name, std::string file_name);
	sf::Font& getFont(std::string name);

	void loadSound(std::string name, std::string file_name);
	sf::SoundBuffer& getSound(std::string name);

	void loadMusic(std::string name, std::string file_name);
	std::unique_ptr<sf::Music>& getMusic(std::string name);

private:
	std::map<std::string, sf::Texture> textures;
	std::map<std::string, sf::Font> fonts;
	std::map<std::string, sf::SoundBuffer> sounds;
	std::map<std::string, std::unique_ptr<sf::Music>> music;
};
