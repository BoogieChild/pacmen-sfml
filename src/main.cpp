#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
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
    window.setFramerateLimit(144);

    const sf::Font bitFont("assets/PressStart2P.ttf");

    sf::Text currTile(bitFont, "Current Tile: ");
    currTile.move({1000, 50});

    //sf::CircleShape pacman(20.0f);
    //pacman.setFillColor(sf::Color(255, 255, 0));
    //pacman.move({(mazeMap.tileSize * 1.0f), (mazeMap.tileSize * 1.0f)});

    const sf::Texture pinkPacmanMovement("assets/pink_pac_man_movement32.png");

    sf::Sprite pinkPacmanSpriteOne(pinkPacmanMovement, {{0, 0}, {32, 32}});

    int x = 0;
    int y = 0;
    
    sf::Text tileX(bitFont, "0");
    tileX.move({1000, 100});

    sf::Text tileY(bitFont, "0");
    tileY.move({1100, 100});

    sf::Text tileId(bitFont, "");
    tileId.move({1000, 150});

    int tileInt = 0;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        int intTileId = mazeMap.getTileId(0, 0);
        tileId.setString(std::to_string(intTileId));

        window.clear(); 

        window.draw(map);
        window.draw(currTile);
        window.draw(tileX);
        window.draw(tileY);
        window.draw(tileId);
        window.draw(pinkPacmanSpriteOne);

        //window.draw(pacman);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
            pinkPacmanSpriteOne.move({0, -5.0f});
            y--;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            pinkPacmanSpriteOne.move({-5.0f, 0});
            x--;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
            pinkPacmanSpriteOne.move({0, 5.0f});
            y++;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            pinkPacmanSpriteOne.move({5.0f, 0});
            x++;
        }

        tileX.setString(std::to_string(x));
        tileY.setString(std::to_string(y));

        window.display();
    }
}
