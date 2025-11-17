#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <fstream>
#include <iostream>
#include <SFML/Audio.hpp>
#include <string>
#include <stdexcept>

#include "Game.h"
#include "Blinky.h"
#include "Clyde.h"
#include "Entity.h"
#include "Inky.h"
#include "MazeMap.h"
#include "Pinky.h"
#include "ResourceManager.h"
#include "Pacman.h"

json Game::loadConfig(const std::filesystem::path& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + configPath.string());
    }

    json data;

    try {
        file >> data;
        return data;
    } catch (const json::parse_error& e) {
        throw std::runtime_error("JSON parse error: " + std::string(e.what()));
    }
}

Game::Game(const std::filesystem::path& configPath) : 
    config(loadConfig(configPath)),
    framerate(static_cast<float>(config["gameConstants"]["frameRate"])),
    perPixelMove(static_cast<float>(config["gameConstants"]["baseSpeedPixelsPerSecond"]) / static_cast<float>(config["gameConstants"]["frameRate"])),
    baseTileSize(static_cast<int>(config["gameConstants"]["tileSize"]))
{};

void Game::run() {
    const int tileSize = baseTileSize * scaleFactor;
    const float perLoopMove = perPixelMove * scaleFactor;

    ResourceManager resources;
    MazeMap map;

    resources.loadTexture("all_textures", "assets/textures/all_textures_transparent.png");
    // resize all_textures to 32px
    resources.resizeTexture("all_textures", tileSize);
    // Collision data: 1=wall, 0=path
    resources.loadMap("mazeMap", "assets/game/maze.txt");
    // Pellet data: 0=none, 1=pellet, 2=power
    resources.loadMap("pelletMap", "assets/game/pellets.txt");

    resources.loadFont("bitFont", "assets/fonts/PressStart2P.ttf");

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

    // Texture layout: [Pellet Maze Image] + gap (4px at base scale) + [Base Maze Image]
    // Original: 224px (28*8) + 4px gap at 8px tiles
    // Gap scales with tile size: (28 * tileSize) + (4 * scaleFactor)
    const unsigned int mazePixelWidth = 28 * tileSize;
    const unsigned int gapWidth = 4 * scaleFactor;
    map.loadMaze(resources.getMazeMap(),
                 resources.getPelletMap(),
                 resources.getTexture("all_textures"),
                 tileSize,
                 {mazePixelWidth + gapWidth, 0},
                 {0, 0});


    Pacman pacman;

    Blinky blinky;
    Pinky pinky;
    Inky inky;
    Clyde clyde;

    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1368, 0}, "right_walking", {45, 45}, 2, 0);
    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1368, 48}, "left_walking", {45, 45}, 2, 0);
    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1368, 96}, "up_walking", {45, 45}, 2, 0);
    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1368, 144}, "down_walking", {45, 45}, 2, 0);
    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1464, 0}, "static", {45, 45}, 1, 0);
    pacman.setAnimationTiles(resources.getTexture("all_textures"), {1512, 0}, "death", {45, 45}, 11, 3);
    pacman.setActiveSprite("static", 0);
    pacman.setOrigin({22.5, 22.5});
    // Tile center = tile * tileSize + tileSize/2
    pacman.setPosition({14.5f * tileSize - (tileSize / 2.0f), 24.0f * tileSize - (tileSize / 2.0f)});
    pacman.setMovementSpeed({perLoopMove, perLoopMove});

    blinky.setAnimationTiles(resources.getTexture("all_textures"), {1368, 192}, "right_walking", {45, 45}, 2, 3);
    blinky.setAnimationTiles(resources.getTexture("all_textures"), {1464, 192}, "left_walking", {45, 45}, 2, 3);
    blinky.setAnimationTiles(resources.getTexture("all_textures"), {1560, 192}, "up_walking", {45, 45}, 2, 3);
    blinky.setAnimationTiles(resources.getTexture("all_textures"), {1656, 192}, "down_walking", {45, 45}, 2, 3);
    blinky.setActiveSprite("left_walking", 0);
    blinky.setOrigin({22.5, 22.5});
    blinky.setPosition({14.5f * tileSize - (tileSize / 2.0f), 12.0f * tileSize - (tileSize / 2.0f)});

    pinky.setAnimationTiles(resources.getTexture("all_textures"), {1368, 240}, "right_walking", {45, 45}, 2, 3);
    pinky.setAnimationTiles(resources.getTexture("all_textures"), {1464, 240}, "left_walking", {45, 45}, 2, 3);
    pinky.setAnimationTiles(resources.getTexture("all_textures"), {1560, 240}, "up_walking", {45, 45}, 2, 3);
    pinky.setAnimationTiles(resources.getTexture("all_textures"), {1656, 240}, "down_walking", {45, 45}, 2, 3);
    pinky.setActiveSprite("down_walking", 0);
    pinky.setOrigin({22.5, 22.5});
    pinky.setPosition({14.5f * tileSize - (tileSize / 2.0f), 15.0f * tileSize - (tileSize / 2.0f)});

    inky.setAnimationTiles(resources.getTexture("all_textures"), {1368, 288}, "right_walking", {45, 45}, 2, 3);
    inky.setAnimationTiles(resources.getTexture("all_textures"), {1464, 288}, "left_walking", {45, 45}, 2, 3);
    inky.setAnimationTiles(resources.getTexture("all_textures"), {1560, 288}, "up_walking", {45, 45}, 2, 3);
    inky.setAnimationTiles(resources.getTexture("all_textures"), {1656, 288}, "down_walking", {45, 45}, 2, 3);
    inky.setActiveSprite("up_walking", 0);
    inky.setOrigin({22.5, 22.5});
    inky.setPosition({12.5f * tileSize - (tileSize / 2.0f), 15.0f * tileSize - (tileSize / 2.0f)});

    clyde.setAnimationTiles(resources.getTexture("all_textures"), {1368, 336}, "right_walking", {45, 45}, 2, 3);
    clyde.setAnimationTiles(resources.getTexture("all_textures"), {1464, 336}, "left_walking", {45, 45}, 2, 3);
    clyde.setAnimationTiles(resources.getTexture("all_textures"), {1560, 336}, "up_walking", {45, 45}, 2, 3);
    clyde.setAnimationTiles(resources.getTexture("all_textures"), {1656, 336}, "down_walking", {45, 45}, 2, 3);
    clyde.setActiveSprite("up_walking", 0);
    clyde.setOrigin({22.5, 22.5});
    clyde.setPosition({16.5f * tileSize - (tileSize / 2.0f), 15.0f * tileSize - (tileSize / 2.0f)});

    auto window = sf::RenderWindow(sf::VideoMode(windowRes), windowName);

    sf::Sound sound(*resources.getSound("start"));
    //sound.play();

    sf::Sound pellet0(*resources.getSound("eat_dot_0"));
    sf::Sound pellet1(*resources.getSound("eat_dot_1"));

    sf::Sound fright(*resources.getSound("fright"));

    int pelletSoundCount = 0;

    sf::Clock clock;

    const sf::Time FIXED_TIMESTEP = sf::seconds(1.0f / framerate);
    sf::Time accumulator = sf::Time::Zero;

    sf::Time initialGameplayPause;

    int score = 0;
    sf::Font bitFont = resources.getFont("bitFont");
    sf::Text scoreText(bitFont);
    scoreText.setString(std::to_string(score));
    scoreText.setPosition({16.0f, tileSize * 31.0f + 16.0f});

    sf::Sprite pacmanLifeOne(resources.getTexture("all_textures"), {{1755, 51}, {39, 39}});
    pacmanLifeOne.setOrigin({39, 0});
    pacmanLifeOne.setPosition({tileSize * 28.0f, tileSize * 31.0f});

    sf::Sprite pacmanLifeTwo(resources.getTexture("all_textures"), {{1755, 51}, {39, 39}});
    pacmanLifeTwo.setOrigin({39, 0});
    pacmanLifeTwo.setPosition({tileSize * 26.5f, tileSize * 31.0f});

    sf::Sprite fruitOne(resources.getTexture("all_textures"), {{1467, 144}, {45, 45}});
    fruitOne.setOrigin({45, 0});
    fruitOne.setPosition({tileSize * 16.0f, tileSize * 31.0f});

    sf::Sprite fruitTwo(resources.getTexture("all_textures"), {{1467, 144}, {45, 45}});
    fruitTwo.setOrigin({45, 0});
    fruitTwo.setPosition({tileSize * 14.0f, tileSize * 31.0f});

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        sf::Time elapsedTime = clock.restart();
        accumulator += elapsedTime;
        initialGameplayPause += elapsedTime;

        while (accumulator >= FIXED_TIMESTEP) {
            accumulator -= FIXED_TIMESTEP;

            if (initialGameplayPause > sf::seconds(4.5f)) {
                sf::Vector2i currentPacmanTile = map.getTileCoords(pacman.getPosition());

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
                    pacman.queueDirection(MovementDir::UP);
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
                    pacman.queueDirection(MovementDir::LEFT);
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
                    pacman.queueDirection(MovementDir::DOWN);
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
                    pacman.queueDirection(MovementDir::RIGHT);
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
                    pacman.queueDirection(MovementDir::STATIC);
                }

                MovementDir queued = pacman.getQueuedDirection();
                if (queued != MovementDir::STATIC && queued != pacman.getCurrentDirection()) {
                    sf::Vector2f currentPos = pacman.getPosition();
                    sf::Vector2i currentTile = map.getTileCoords(currentPos);

                    const float corneringTolerance = 4.0f;
                    const float halfTile = tileSize / 2.0f;
                    bool withinTolerance = false;

                    if (queued == MovementDir::UP || queued == MovementDir::DOWN) {
                        float tileCenterX = currentTile.x * tileSize + halfTile;
                        withinTolerance = std::abs(currentPos.x - tileCenterX) <= corneringTolerance;
                    } else if (queued == MovementDir::LEFT || queued == MovementDir::RIGHT) {
                        float tileCenterY = currentTile.y * tileSize + halfTile;
                        withinTolerance = std::abs(currentPos.y - tileCenterY) <= corneringTolerance;
                    }

                    if (withinTolerance) {
                        sf::Vector2i targetTile = currentTile;

                        switch (queued) {
                            case MovementDir::UP:
                                targetTile.y -= 1;
                                break;
                            case MovementDir::DOWN:
                                targetTile.y += 1;
                                break;
                            case MovementDir::LEFT:
                                targetTile.x -= 1;
                                if (targetTile.x < 0) targetTile.x = map.getWidth() - 1;
                                break;
                            case MovementDir::RIGHT:
                                targetTile.x += 1;
                                if (targetTile.x >= static_cast<int>(map.getWidth())) targetTile.x = 0;
                                break;
                            case MovementDir::STATIC:
                                break;
                        }

                        if (!map.isWall(targetTile)) {
                            map.snapEntityToGrid(pacman);

                            switch (queued) {
                                case MovementDir::UP:
                                    pacman.setActiveSprite("up_walking", 0);
                                    break;
                                case MovementDir::DOWN:
                                    pacman.setActiveSprite("down_walking", 0);
                                    break;
                                case MovementDir::LEFT:
                                    pacman.setActiveSprite("left_walking", 0);
                                    break;
                                case MovementDir::RIGHT:
                                    pacman.setActiveSprite("right_walking", 0);
                                    break;
                                case MovementDir::STATIC:
                                    break;
                            }

                            sf::Vector2f targetCenter = map.getTargetTileCenter(targetTile);
                            pacman.startMove(queued, targetCenter);
                            pacman.clearQueuedDirection();
                        }
                    }
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

                //check if blinky is at an intersection
                //if yes set target tile to pacmans tile
                //calculate distance for all legal move options
                //choose option with least distance for blinky to pacman


                map.handleTunnelWrapping(pacman);

                if (map.hasPellet(currentPacmanTile)) {
                    map.eatPellet(currentPacmanTile);

                    switch (map.getPelletType(currentPacmanTile)) {
                        case PelletType::NONE:
                            break;
                        case PelletType::DOT:
                            score += 10;
                            break;
                        case PelletType::ENERGIZER:
                            score += 50;
                            //fright.play();
                            break;
                    };

                    scoreText.setString(std::to_string(score));

                    if (pelletSoundCount % 2) {
                        //pellet1.play();
                    } else {
                        //pellet0.play();
                    }

                    pelletSoundCount++;
                }
            }
        }

        window.clear();

        window.draw(map);
        window.draw(pacman);
        window.draw(blinky);
        window.draw(inky);
        window.draw(pinky);
        window.draw(clyde);
        window.draw(scoreText);
        window.draw(pacmanLifeOne);
        window.draw(pacmanLifeTwo);
        window.draw(fruitOne);
        window.draw(fruitTwo);

        window.display();
    }

};
