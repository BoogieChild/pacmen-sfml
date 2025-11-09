#include "ResourceManager.h"
#include "MazeMap.h"
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <fstream>
#include <vector>

bool ResourceManager::loadTexture(const std::string& textureName, const std::filesystem::path& texturePath) {
    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(texturePath)) return false;

    textures[textureName] = std::move(texture);
    return true;
};

bool ResourceManager::loadFont(const std::string& fontName, const std::filesystem::path& fontPath) {
    sf::Font font;
    if (!font.openFromFile(fontPath)) return false;

    fonts[fontName] = font;

    return true;
};

bool ResourceManager::loadMap(const std::string& mapName, const std::filesystem::path& mapPath) {
    std::ifstream file(mapPath);
    if (!file.is_open()) return false;

    std::vector<int> mapData;
    int value;

    while (file >> value) {
        mapData.push_back(value);
    }

    file.close();

    if (mapName == "mazeMap") mazeMap = mapData;
    if (mapName == "pelletMap") pelletMap = mapData;

    return !mapData.empty();
};

bool ResourceManager::loadSound(const std::string& soundName, const std::filesystem::path& soundPath) {
    sounds[soundName] = sf::SoundBuffer(soundPath);

    return true;
};


void ResourceManager::resizeTexture(const std::string& textureName, unsigned int newTextureTileSize) {
    auto it = textures.find(textureName);
    if (it == textures.end()) return;

    sf::Texture& originalTexture = *it->second;

    originalTexture.setSmooth(false);

    float scaleFactor = static_cast<float>(newTextureTileSize) / static_cast<float>(textureTileSize);

    sf::Vector2u originalSize = originalTexture.getSize();
    sf::Vector2u newSize(
        static_cast<unsigned int>(originalSize.x * scaleFactor),
        static_cast<unsigned int>(originalSize.y * scaleFactor)
    );

    sf::RenderTexture renderTexture(newSize);

    renderTexture.clear(sf::Color::Transparent);

    sf::Sprite sprite(originalTexture);
    sprite.setScale({scaleFactor, scaleFactor});
    renderTexture.draw(sprite);
    renderTexture.display();

    auto newTexture = std::make_unique<sf::Texture>(renderTexture.getTexture());
    newTexture->setSmooth(false);

    textures[textureName] = std::move(newTexture);
};
