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

#include "Entity.h"
#include "TileMap.h"
//#include "MazeCharacter.h"

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

    if (!mazeMap.load("assets/maze32.png",
                  {mazeMap.tileSize, mazeMap.tileSize},
                  mazeMap.tiles.data(),
                  mazeMap.width,
                  mazeMap.height))
        return -1;

    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "Pacmen");
    window.setFramerateLimit(60);

    int tileSize = mazeMap.tileSize;
    int halfTile = tileSize / 2;

    int legalTileId = 92;

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

    sf::Text tileIdText(bitFont, "Tile ID: ");
    tileIdText.move({1000, 350});

    sf::Text tileId(bitFont, "");
    tileId.move({1000, 400});

    sf::Text movementDirText(bitFont, "Movement Direction: ");
    movementDirText.move({1000, 450});

    sf::Text movementDir(bitFont, "STATIC");
    movementDir.move({1000, 500});

    const sf::Texture pinkPacmanMovement("assets/pink_pac_man_movement48.png");
    sf::Sprite pinkPacmanSpriteOne(pinkPacmanMovement, {{0, 0}, {48, 48}});

    // 24 tiles down, 13-14 tiles right
    pinkPacmanSpriteOne.move({(13.0f * tileSize) + (halfTile / 2.0f), (23.0f * tileSize) - (halfTile / 2.0f)});

    sf::Vector2f pacmanCenter;

    sf::Vector2i pacmanVelo;

    sf::Vector2i transformVeloUp({0, -1});
    sf::Vector2i transformVeloDown({0, 1});
    sf::Vector2i transformVeloRight({1, 0});
    sf::Vector2i transformVeloLeft({-1, 0});

    float pacmanSpeed = 5;

    //MazeCharacter pacman("assets/pink_pac_man_movement48.png", mazeMap);

    Entity pacman(mazeMap);
    pacman.loadTexture("assets/pink_pac_man_movement48.png", {48, 48}, 5);

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

        window.draw(mazeMap);
        window.draw(currTile);
        window.draw(tileX);
        window.draw(tileY);
        window.draw(currTileX);
        window.draw(currTileY);
        window.draw(pinkPacmanSpriteOne);
        window.draw(pacCent);
        window.draw(pacCentX);
        window.draw(pacCentY);
        window.draw(tileIdText);
        window.draw(tileId);
        window.draw(movementDirText);
        window.draw(movementDir);

        window.draw(pacman);

        pacmanCenter = {pinkPacmanSpriteOne.getPosition().x + 16.0f, pinkPacmanSpriteOne.getPosition().y + 16.0f};

        sf::Vector2i pacmanTile = {(int)std::round((pacmanCenter.x - 16) / 32), (int)std::round((pacmanCenter.y - 16) / 32)};

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            pacmanVelo = transformVeloUp;
            movementDir.setString("UP");
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            pacmanVelo = transformVeloLeft;
            movementDir.setString("LEFT");
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)|| sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            pacmanVelo = transformVeloDown;
            movementDir.setString("DOWN");
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            pacmanVelo = transformVeloRight;
            movementDir.setString("RIGHT");
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
            pacmanVelo = {0, 0};
            movementDir.setString("STATIC");
        }

        // center coord 16, 16
        // tile 0, 0
        // center coord 48, 48
        // tile 1, 1
        // center coord 80, 80
        // tile 2, 2
        // (cent - 16) / 32

        tileX.setString(std::to_string(pacmanTile.x));
        tileY.setString(std::to_string(pacmanTile.y));

        pacCentX.setString(std::to_string(pacmanCenter.x));
        pacCentY.setString(std::to_string(pacmanCenter.y));

        tileId.setString(std::to_string(mazeMap.getTileId(pacmanTile.x, pacmanTile.y)));

        //if (mazeMap.isLegalTile(pacmanTile + pacmanVelo)) {
        //    sf::Vector2f pacmanVeloFloat = {pacmanVelo.x * 1.0f, pacmanVelo.y * 1.0f};
        //    pinkPacmanSpriteOne.move(pacmanSpeed * pacmanVeloFloat);
        //}

        sf::Vector2f pacmanVeloFloat = {pacmanVelo.x * 1.0f, pacmanVelo.y * 1.0f};
        pinkPacmanSpriteOne.move(pacmanSpeed * pacmanVeloFloat);
        pacman.move(pacmanSpeed * pacmanVeloFloat);

        window.display();
    }
}
