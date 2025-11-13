#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <fstream>
#include <iostream>
#include <SFML/Audio.hpp>
#include <string>
#include <stdexcept>

#include "Game.h"
#include "MazeMap.h"
#include "ResourceManager.h"
#include "Pacman.h"
#include "Ghost.h"

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

    // Texture layout: [Pellet Maze Image] + gap (16px) + [Base Maze Image]
    // Original: 224px (28*8) + 4px gap at 8px tiles
    // Scaled 4x: 896px (28*32) + 16px gap at 32px tiles
    map.loadMaze(resources.getMazeMap(),
                 resources.getPelletMap(),
                 resources.getTexture("all_textures"),
                 tileSize,
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
    pacman.setMovementSpeed({perLoopMove, perLoopMove});

    blinky.setAnimationTiles(resources.getTexture("all_textures"), {1824, 256}, "right_walking", {60, 60}, 2, 4);
    blinky.setAnimationTiles(resources.getTexture("all_textures"), {1952, 256}, "left_walking", {60, 60}, 2, 4);
    blinky.setAnimationTiles(resources.getTexture("all_textures"), {2080, 256}, "up_walking", {60, 60}, 2, 4);
    blinky.setAnimationTiles(resources.getTexture("all_textures"), {2208, 256}, "down_walking", {60, 60}, 2, 4);
    blinky.setActiveSprite("left_walking", 0);
    blinky.setOrigin({30, 30});
    blinky.setPosition({14.5 * 32.0f - 16.0f, 12 * 32.0f - 16.0f});

    pinky.setAnimationTiles(resources.getTexture("all_textures"), {1824, 320}, "right_walking", {60, 60}, 2, 4);
    pinky.setAnimationTiles(resources.getTexture("all_textures"), {1952, 320}, "left_walking", {60, 60}, 2, 4);
    pinky.setAnimationTiles(resources.getTexture("all_textures"), {2080, 320}, "up_walking", {60, 60}, 2, 4);
    pinky.setAnimationTiles(resources.getTexture("all_textures"), {2208, 320}, "down_walking", {60, 60}, 2, 4);
    pinky.setActiveSprite("down_walking", 0);
    pinky.setOrigin({30, 30});
    pinky.setPosition({14.5 * 32.0f - 16.0f, 15 * 32.0f - 16.0f});

    inky.setAnimationTiles(resources.getTexture("all_textures"), {1824, 384}, "right_walking", {60, 60}, 2, 4);
    inky.setAnimationTiles(resources.getTexture("all_textures"), {1952, 384}, "left_walking", {60, 60}, 2, 4);
    inky.setAnimationTiles(resources.getTexture("all_textures"), {2080, 384}, "up_walking", {60, 60}, 2, 4);
    inky.setAnimationTiles(resources.getTexture("all_textures"), {2208, 384}, "down_walking", {60, 60}, 2, 4);
    inky.setActiveSprite("up_walking", 0);
    inky.setOrigin({30, 30});
    inky.setPosition({12.5 * 32.0f - 16.0f, 15 * 32.0f - 16.0f});

    clyde.setAnimationTiles(resources.getTexture("all_textures"), {1824, 448}, "right_walking", {60, 60}, 2, 4);
    clyde.setAnimationTiles(resources.getTexture("all_textures"), {1952, 448}, "left_walking", {60, 60}, 2, 4);
    clyde.setAnimationTiles(resources.getTexture("all_textures"), {2080, 448}, "up_walking", {60, 60}, 2, 4);
    clyde.setAnimationTiles(resources.getTexture("all_textures"), {2208, 448}, "down_walking", {60, 60}, 2, 4);
    clyde.setActiveSprite("up_walking", 0);
    clyde.setOrigin({30, 30});
    clyde.setPosition({16.5 * 32.0f - 16.0f, 15 * 32.0f - 16.0f});

    auto window = sf::RenderWindow(sf::VideoMode(windowRes), windowName);

    sf::Sound sound(*resources.getSound("start"));
    sound.play();

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
    scoreText.setPosition({16, 32 * 31 + 16});

    sf::Sprite pacmanLifeOne(resources.getTexture("all_textures"), {{2340, 68}, {52, 52}});
    pacmanLifeOne.setOrigin({52, 0});
    pacmanLifeOne.setPosition({32 * 28, 32 * 31});
    
    sf::Sprite pacmanLifeTwo(resources.getTexture("all_textures"), {{2340, 68}, {52, 52}});
    pacmanLifeTwo.setOrigin({52, 0});
    pacmanLifeTwo.setPosition({32 * 26.5, 32 * 31});

    sf::Sprite fruitOne(resources.getTexture("all_textures"), {{1956, 192}, {60, 60}});
    fruitOne.setOrigin({60, 0});
    fruitOne.setPosition({32 * 16, 32 * 31});

    sf::Sprite fruitTwo(resources.getTexture("all_textures"), {{1956, 192}, {60, 60}});
    fruitTwo.setOrigin({60, 0});
    fruitTwo.setPosition({32 * 14, 32 * 31});

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

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
                    pacman.queueDirection(MovementDir::UP);
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
                    pacman.queueDirection(MovementDir::LEFT);
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
                    pacman.queueDirection(MovementDir::DOWN);
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
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

                        if (!map.isWall(targetTile.x, targetTile.y)) {
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

                map.handleTunnelWrapping(pacman);

                if (map.hasPellet(currentPacmanTile.x, currentPacmanTile.y)) {
                    map.eatPellet(currentPacmanTile.x, currentPacmanTile.y);

                    switch (map.getPelletType(currentPacmanTile)) {
                        case PelletType::NONE:
                            break;
                        case PelletType::DOT:
                            score += 10;
                            break;
                        case PelletType::ENERGIZER:
                            score += 50;
                            fright.play();
                            break;
                    };

                    scoreText.setString(std::to_string(score));

                    if (pelletSoundCount % 2) {
                        pellet1.play();
                    } else {
                        pellet0.play();
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
