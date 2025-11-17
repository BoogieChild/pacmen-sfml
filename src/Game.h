#ifndef GAME_H
#define GAME_H

#include <SFML/System/Vector2.hpp>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Game {
public:
    Game(const std::filesystem::path& configPath);
    
    void setScaleFactor(int newScaleFactor) { scaleFactor = newScaleFactor; }

    void run();

private:
    json loadConfig(const std::filesystem::path& configPath);

    const json config;

    const float framerate;
    const float perPixelMove;
    const int baseTileSize;

    int scaleFactor = 3;

    sf::Vector2u windowRes = {672, 810};
    std::string windowName = "Pacmen";
};

#endif
