#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <SFML/Audio.hpp>

#include "Game.h"
#include "MazeMap.h"
#include "ResourceManager.h"
#include "Pacman.h"
#include "Ghost.h"

void Game::run() {
    ResourceManager resources;
    MazeMap map;

    resources.loadTexture("all_textures", "assets/textures/all_textures_transparent.png");
    // resize all_textures to 32px
    resources.resizeTexture("all_textures", 32);
    // Collision data: 1=wall, 0=path
    resources.loadMap("mazeMap", "assets/game/maze.txt");
    // Pellet data: 0=none, 1=pellet, 2=power
    resources.loadMap("pelletMap", "assets/game/pellets.txt");

    resources.loadSound("credit", "assets/sounds/credit.wav");
    resources.loadSound("death_0", "assets/sounds/death_0.wav");
    resources.loadSound("death_1", "assets/sounds/death_1.wav");
    resources.loadSound("eat_dot_0", "assets/sounds/eat_dot_0.wav");
    resources.loadSound("eat_dot_1", "assets/sounds/eat_dot_1.wav");
    resources.loadSound("eat_fruit", "assets/sounds/eat_fruit.wav");
    resources.loadSound("eat_ghost", "assets/sounds/eat_ghost.wav");
    resources.loadSound("extend", "assets/sounds/extend.wav");
    resources.loadSound("eyes", "assets/sounds/eyes.wav");
    resources.loadSound("eyes_firstloop", "assets/sounds/eyes_firstloop.wav");
    resources.loadSound("fright", "assets/sounds/fright.wav");
    resources.loadSound("fright_firstloop", "assets/sounds/fright_firstloop.wav");
    resources.loadSound("intermission.wav", "assets/sounds/intermission.wav");
    resources.loadSound("siren0", "assets/sounds/siren0.wav");
    resources.loadSound("siren0_firstloop", "assets/sounds/siren0_firstloop.wav");
    resources.loadSound("siren1", "assets/sounds/siren1.wav");
    resources.loadSound("siren1_firstloop", "assets/sounds/siren1_firstloop.wav");
    resources.loadSound("siren2", "assets/sounds/siren2.wav");
    resources.loadSound("siren2_firstloop", "assets/sounds/siren2_firstloop.wav");
    resources.loadSound("siren3", "assets/sounds/siren3.wav");
    resources.loadSound("siren3_firstloop", "assets/sounds/siren3_firstloop.wav");
    resources.loadSound("siren4", "assets/sounds/siren4.wav");
    resources.loadSound("siren4_firstloop", "assets/sounds/siren4_firstloop.wav");
    resources.loadSound("start", "assets/sounds/start.wav");

    // Texture layout: [Pellet Maze Image] + gap (16px) + [Base Maze Image]
    // Original: 224px (28*8) + 4px gap at 8px tiles
    // Scaled 4x: 896px (28*32) + 16px gap at 32px tiles
    map.loadMaze(resources.getMazeMap(),
                 resources.getPelletMap(),
                 resources.getTexture("all_textures"),
                 32,
                 {912, 0},
                 {0, 0});

    Pacman pacman;

    Ghost blinky;
    Ghost pinky;
    Ghost inky;
    Ghost clyde;

    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1824, 0}, "right_walking", {60, 60}, 2, 0);
    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1824, 64}, "left_walking", {60, 60}, 2, 0);
    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1824, 128}, "up_walking", {60, 60}, 2, 0);
    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1824, 192}, "down_walking", {60, 60}, 2, 0);
    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1952, 0}, "static", {60, 60}, 1, 0);
    pacman.setAnimationTiles(resources.getTexture("all_textures"), {2016, 0}, "death", {60, 60}, 11, 4);
    pacman.setActiveSprite("static", 0);
    pacman.setOrigin({30, 30});
    // Tile center = tile * tileSize + tileSize/2
    pacman.setPosition({14.5 * 32.0f - 16.0f, 24 * 32.0f - 16.0f});

    blinky.setAnimationTiles(resources.getTexture("all_textures"), {1824, 256}, "right_walking", {60, 60}, 2, 4);
    blinky.setAnimationTiles(resources.getTexture("all_textures"), {1952, 256}, "left_walking", {60, 60}, 2, 4);
    blinky.setAnimationTiles(resources.getTexture("all_textures"), {2080, 256}, "up_walking", {60, 60}, 2, 4);
    blinky.setAnimationTiles(resources.getTexture("all_textures"), {2208, 256}, "down_walking", {60, 60}, 2, 4);
    blinky.setActiveSprite("right_walking", 0);

    pinky.setAnimationTiles(resources.getTexture("all_textures"), {1824, 320}, "right_walking", {60, 60}, 2, 4);
    pinky.setAnimationTiles(resources.getTexture("all_textures"), {1952, 320}, "left_walking", {60, 60}, 2, 4);
    pinky.setAnimationTiles(resources.getTexture("all_textures"), {2080, 320}, "up_walking", {60, 60}, 2, 4);
    pinky.setAnimationTiles(resources.getTexture("all_textures"), {2208, 320}, "down_walking", {60, 60}, 2, 4);
    pinky.setActiveSprite("right_walking", 0);

    inky.setAnimationTiles(resources.getTexture("all_textures"), {1824, 384}, "right_walking", {60, 60}, 2, 4);
    inky.setAnimationTiles(resources.getTexture("all_textures"), {1952, 384}, "left_walking", {60, 60}, 2, 4);
    inky.setAnimationTiles(resources.getTexture("all_textures"), {2080, 384}, "up_walking", {60, 60}, 2, 4);
    inky.setAnimationTiles(resources.getTexture("all_textures"), {2208, 384}, "down_walking", {60, 60}, 2, 4);
    inky.setActiveSprite("right_walking", 0);

    clyde.setAnimationTiles(resources.getTexture("all_textures"), {1824, 448}, "right_walking", {60, 60}, 2, 4);
    clyde.setAnimationTiles(resources.getTexture("all_textures"), {1952, 448}, "left_walking", {60, 60}, 2, 4);
    clyde.setAnimationTiles(resources.getTexture("all_textures"), {2080, 448}, "up_walking", {60, 60}, 2, 4);
    clyde.setAnimationTiles(resources.getTexture("all_textures"), {2208, 448}, "down_walking", {60, 60}, 2, 4);
    clyde.setActiveSprite("right_walking", 0);

    auto window = sf::RenderWindow(sf::VideoMode(windowRes), windowName);
    window.setFramerateLimit(maxFPS);

    sf::Sound sound(*resources.getSound("start"));
    sound.play();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            pacman.queueDirection(MovementDir::UP);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            pacman.queueDirection(MovementDir::LEFT);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)|| sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            pacman.queueDirection(MovementDir::DOWN);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            pacman.queueDirection(MovementDir::RIGHT);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
            pacman.queueDirection(MovementDir::STATIC);
        }

        if (!pacman.isCurrentlyMoving()) {
            MovementDir nextDir = MovementDir::STATIC;

            MovementDir queued = pacman.getQueuedDirection();
            if (queued != MovementDir::STATIC && map.entityCanMove(pacman, queued)) {
                nextDir = queued;
                pacman.clearQueuedDirection();
            }
            else if (pacman.getCurrentDirection() != MovementDir::STATIC &&
                     map.entityCanMove(pacman, pacman.getCurrentDirection())) {
                nextDir = pacman.getCurrentDirection();
            }

            if (nextDir != MovementDir::STATIC) {
                sf::Vector2i currentTile = map.getTileCoords(pacman.getPosition());
                sf::Vector2i targetTile = currentTile;

                switch (nextDir) {
                    case MovementDir::UP:
                        targetTile.y -= 1;
                        pacman.setActiveSprite("up_walking", 0);
                        break;
                    case MovementDir::DOWN:
                        targetTile.y += 1;
                        pacman.setActiveSprite("down_walking", 0);
                        break;
                    case MovementDir::LEFT:
                        targetTile.x -= 1;
                        pacman.setActiveSprite("left_walking", 0);
                        break;
                    case MovementDir::RIGHT:
                        targetTile.x += 1;
                        pacman.setActiveSprite("right_walking", 0);
                        break;
                    case MovementDir::STATIC:
                        break;
                }

                sf::Vector2f targetCenter = map.getTargetTileCenter(targetTile);
                pacman.startMove(nextDir, targetCenter);
            } else {
                pacman.setActiveSprite("static", 0);
            }
        }

        pacman.update();

        window.clear();
        window.draw(map);
        window.draw(pacman);
        window.display();
    }

};
