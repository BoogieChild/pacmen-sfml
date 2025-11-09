#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <map>
#include <vector>
#include <filesystem>
#include <memory>

#include <SFML/Graphics/Texture.hpp>
//#include <SFML/Audio/SoundBuffer.hpp>

#include "MazeMap.h"

class ResourceManager {
public:
    ResourceManager() : textureTileSize(8) {};

    bool loadTexture(const std::string& textureName, const std::filesystem::path& texturePath);

    bool loadFont(const std::string& fontName, const std::filesystem::path& fontPath);

    bool loadMap(const std::string& mapName, const std::filesystem::path& mapPath);

    bool loadSound(const std::string& soundName, const std::filesystem::path& soundPath);

    sf::Texture& getTexture(const std::string& textureName) {
        return *textures[textureName];
    };

    void resizeTexture(const std::string& textureName, unsigned int newTextureTileSize);

    std::vector<int>& getMazeMap() {
        return mazeMap;
    }

    std::vector<int>& getPelletMap() {
        return pelletMap;
    }

    sf::SoundBuffer* getSound(const std::string& soundName) {
        return &sounds[soundName];
    };

    //load gameplay info

private:
    //textures and sprites
    std::map<std::string, std::unique_ptr<sf::Texture>> textures;
    const int textureTileSize;

    //fonts 
    std::map<std::string, sf::Font> fonts;

    //sounds
    std::map<std::string, sf::SoundBuffer> sounds;

    //map 
    std::vector<int> mazeMap;
    std::vector<int> pelletMap;

    //gameplay info (speeds, scores, etc.)
};

#endif
