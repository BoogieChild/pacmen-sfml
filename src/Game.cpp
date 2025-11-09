#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>

#include "Game.h"
#include "MazeMap.h"
#include "ResourceManager.h"
#include "Entity.h"

void Game::run() {
    ResourceManager resources;
    MazeMap map;

    resources.loadTexture("all_textures", "assets/all_textures_transparent.png");
    // resize all_textures to 32px
    resources.resizeTexture("all_textures", 32);
    // Collision data: 1=wall, 0=path
    resources.loadMap("mazeMap", "assets/maze.txt");
    // Pellet data: 0=none, 1=pellet, 2=power
    resources.loadMap("pelletMap", "assets/pellets.txt");

    // Texture layout: [Pellet Maze Image] + gap (16px) + [Base Maze Image]
    // Original: 224px (28*8) + 4px gap at 8px tiles
    // Scaled 4x: 896px (28*32) + 16px gap at 32px tiles
    map.loadMaze(resources.getMazeMap(),
                 resources.getPelletMap(),
                 resources.getTexture("all_textures"),
                 32,
                 {912, 0},
                 {0, 0});

    Entity pacman;

    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1824, 0}, "right_walking", {60, 60}, 2, 0);
    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1824, 64}, "left_walking", {60, 60}, 2, 0);
    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1824, 128}, "up_walking", {60, 60}, 2, 0);
    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1824, 192}, "down_walking", {60, 60}, 2, 0);
    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1952, 0}, "static", {60, 60}, 1, 0);
    pacman.setAnimationTiles(resources.getTexture("all_textures"), {2016, 0}, "death", {60, 60}, 11, 4);
    pacman.setActiveSprite("static", 0);

    Entity blinky;

    blinky.setAnimationTiles(resources.getTexture("all_textures"), {1824, 256}, "right_walking", {60, 60}, 2, 4);
    blinky.setAnimationTiles(resources.getTexture("all_textures"), {1952, 256}, "left_walking", {60, 60}, 2, 4);
    blinky.setAnimationTiles(resources.getTexture("all_textures"), {2080, 256}, "up_walking", {60, 60}, 2, 4);
    blinky.setAnimationTiles(resources.getTexture("all_textures"), {2208, 256}, "down_walking", {60, 60}, 2, 4);
    blinky.setActiveSprite("right_walking", 0);

    Entity pinky;

    pinky.setAnimationTiles(resources.getTexture("all_textures"), {1824, 320}, "right_walking", {60, 60}, 2, 4);
    pinky.setAnimationTiles(resources.getTexture("all_textures"), {1952, 320}, "left_walking", {60, 60}, 2, 4);
    pinky.setAnimationTiles(resources.getTexture("all_textures"), {2080, 320}, "up_walking", {60, 60}, 2, 4);
    pinky.setAnimationTiles(resources.getTexture("all_textures"), {2208, 320}, "down_walking", {60, 60}, 2, 4);
    pinky.setActiveSprite("right_walking", 0);

    Entity inky;

    inky.setAnimationTiles(resources.getTexture("all_textures"), {1824, 384}, "right_walking", {60, 60}, 2, 4);
    inky.setAnimationTiles(resources.getTexture("all_textures"), {1952, 384}, "left_walking", {60, 60}, 2, 4);
    inky.setAnimationTiles(resources.getTexture("all_textures"), {2080, 384}, "up_walking", {60, 60}, 2, 4);
    inky.setAnimationTiles(resources.getTexture("all_textures"), {2208, 384}, "down_walking", {60, 60}, 2, 4);
    inky.setActiveSprite("right_walking", 0);

    Entity clyde;

    clyde.setAnimationTiles(resources.getTexture("all_textures"), {1824, 448}, "right_walking", {60, 60}, 2, 4);
    clyde.setAnimationTiles(resources.getTexture("all_textures"), {1952, 448}, "left_walking", {60, 60}, 2, 4);
    clyde.setAnimationTiles(resources.getTexture("all_textures"), {2080, 448}, "up_walking", {60, 60}, 2, 4);
    clyde.setAnimationTiles(resources.getTexture("all_textures"), {2208, 448}, "down_walking", {60, 60}, 2, 4);
    clyde.setActiveSprite("right_walking", 0);

    auto window = sf::RenderWindow(sf::VideoMode(windowRes), windowName);
    window.setFramerateLimit(maxFPS);

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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            pacman.setActiveSprite("up_walking", 0);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            pacman.setActiveSprite("left_walking", 0);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)|| sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            pacman.setActiveSprite("down_walking", 0);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            pacman.setActiveSprite("right_walking", 0);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
            pacman.setActiveSprite("static", 0);
        }

        window.draw(*pacman.getActiveSprite());

        //window.draw();

        window.display();
    }

};
