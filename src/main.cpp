#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include "TileMapLoader.h"
#include "TileMap.h"

int main()
{
    // Load tilemap data from JSON
    TileMapData tilemapData;
    try {
        tilemapData = TileMapLoader::loadFromJSON("util/pacmantiles.json");
    } catch (const std::exception& e) {
        std::cerr << "Error loading tilemap: " << e.what() << std::endl;
        return -1;
    }

    TileMap map;
    if (!map.load("assets/maze.png",
                  {tilemapData.tileSize, tilemapData.tileSize},
                  tilemapData.tiles.data(),
                  tilemapData.width,
                  tilemapData.height))
        return -1;

    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "Pacmen");
    window.setFramerateLimit(144);

    sf::CircleShape pacman(16.0f);
    pacman.setFillColor(sf::Color(255, 255, 0));

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();

        window.draw(map);

        window.draw(pacman);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
            pacman.move({0, -5.0f});
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            pacman.move({-5.0f, 0});
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
            pacman.move({0, 5.0f});
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            pacman.move({5.0f, 0});
        }

        window.display();
    }
}
