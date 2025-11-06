#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include "TileMap.h"

int main()
{
    // Load tilemap data from JSON
    TileMap mazeMap;
    try {
        mazeMap.loadFromJSON("util/pacmantiles32.json");
    } catch (const std::exception& e) {
        std::cerr << "Error loading tilemap: " << e.what() << std::endl;
        return -1;
    }

    TileMap map;
    if (!map.load("assets/maze32.png",
                  {mazeMap.tileSize, mazeMap.tileSize},
                  mazeMap.tiles.data(),
                  mazeMap.width,
                  mazeMap.height))
        return -1;

    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "Pacmen");
    window.setFramerateLimit(60);

    const sf::Font bitFont("assets/PressStart2P.ttf");

    sf::Text currTile(bitFont, "Current Tile: ");
    currTile.move({1000, 50});

    sf::Text currTileX(bitFont, "X: ");
    currTileX.move({1000, 100});

    sf::Text currTileY(bitFont, "Y: ");
    currTileY.move({1000, 150});

    sf::Text tileX(bitFont, "");
    tileX.move({1100, 100});

    sf::Text tileY(bitFont, "");
    tileY.move({1100, 150});

    sf::Text pacCent(bitFont, "Pacman Center:");
    pacCent.move({1000, 200});

    sf::Text pacCentX(bitFont, "");
    pacCentX.move({1000, 250});

    sf::Text pacCentY(bitFont, "");
    pacCentY.move({1000, 300});

    const sf::Texture pinkPacmanMovement("assets/pink_pac_man_movement48.png");
    sf::Sprite pinkPacmanSpriteOne(pinkPacmanMovement, {{0, 0}, {48, 48}});

    sf::Vector2f pacmanCenter({0, 0});

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
        window.draw(currTile);
        window.draw(tileX);
        window.draw(tileY);
        window.draw(currTileX);
        window.draw(currTileY);
        window.draw(pinkPacmanSpriteOne);
        window.draw(pacCent);
        window.draw(pacCentX);
        window.draw(pacCentY);

        pacmanCenter = {pinkPacmanSpriteOne.getPosition().x + 16.0f, pinkPacmanSpriteOne.getPosition().y + 16.0f};

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            pinkPacmanSpriteOne.move({0, -5.0f});
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            pinkPacmanSpriteOne.move({-5.0f, 0});
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)|| sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            pinkPacmanSpriteOne.move({0, 5.0f});
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            pinkPacmanSpriteOne.move({5.0f, 0});
        }

        // center coord 16, 16
        // tile 0, 0
        // center coord 48, 48
        // tile 1, 1
        // center coord 80, 80
        // tile 2, 2
        // (cent - 16) / 32

        tileX.setString(std::to_string(std::round((pacmanCenter.x - 16) / 32)));
        tileY.setString(std::to_string(std::round((pacmanCenter.y - 16) / 32)));

        pacCentX.setString(std::to_string(pacmanCenter.x));
        pacCentY.setString(std::to_string(pacmanCenter.y));

        window.display();
    }
}
