#include "asset_manager.h"

void AssetManager::loadTexture(std::string name, std::string file_name)
{
	if (textures.find(name) == textures.end())
	{
		sf::Texture tekstura;

		if (tekstura.loadFromFile(file_name))
		{
			textures[name] = tekstura;
		}
	}
}

sf::Texture& AssetManager::getTexture(std::string name)
{
	return textures.at(name);
}

void AssetManager::loadFont(std::string name, std::string file_name)
{
	if (fonts.find(name) == fonts.end())
	{
		sf::Font font;

		if (font.loadFromFile(file_name))
		{
			fonts[name] = font;
		}
	}
}

sf::Font& AssetManager::getFont(std::string name)
{
	return fonts.at(name);
}

void AssetManager::loadSound(std::string name, std::string file_name)
{
	if (sounds.find(name) == sounds.end())
	{
		sf::SoundBuffer dzwiek;

		if (dzwiek.loadFromFile(file_name))
		{
			sounds[name] = dzwiek;
		}
	}
}

sf::SoundBuffer& AssetManager::getSound(std::string name)
{
	return sounds.at(name);
}

void AssetManager::loadMusic(std::string name, std::string file_name)
{
	if (music.find(name) == music.end())
	{
		auto utwor = std::make_unique<sf::Music>();

		if (utwor->openFromFile(file_name))
		{
			music.emplace(name, std::move(utwor));
		}
	}
}

std::unique_ptr<sf::Music>& AssetManager::getMusic(std::string name)
{
	return music.at(name);
}