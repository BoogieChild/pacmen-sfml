#ifndef MAZECHARACTER_H
#define MAZECHARACTER_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include "TileMap.h"

enum class MovementDir {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class MazeCharacter : public sf::Transformable, public sf::Drawable {
public:
    MazeCharacter(const std::filesystem::path& textureFile, TileMap& map, int tileSize = 32, int baseMovementSpeed = 5) : tileSize(tileSize), baseMovementSpeed(baseMovementSpeed), texture(textureFile), mazeMap(map) {
        sprite = new sf::Sprite(texture);
    };

    void changeDirection(MovementDir dir) {
        switch (dir) {
            case MovementDir::UP:
                veloDir = transformVeloUp;
                break;
            case MovementDir::DOWN:
                veloDir = transformVeloDown;
                break;
            case MovementDir::LEFT:
                veloDir = transformVeloLeft;
                break;
            case MovementDir::RIGHT:
                veloDir = transformVeloRight;
                break;
        };
    };

    ~MazeCharacter() {
        delete sprite;
    }

private:
    int tileSize = 0;
    int halfTileSize = tileSize / 2;
    std::vector<int> legalTileIds;

    sf::Texture texture;
    sf::Sprite* sprite;

    sf::Vector2i tilePos;
    sf::Vector2f screenPos;
    sf::Vector2f centerPos = {screenPos.x + halfTileSize, screenPos.y + halfTileSize};
    
    sf::Vector2f velo;
    sf::Vector2i veloDir;

    sf::Vector2i transformVeloUp{0, -1};
    sf::Vector2i transformVeloDown{0, 1};
    sf::Vector2i transformVeloRight{1, 0};
    sf::Vector2i transformVeloLeft{-1, 0};
    
    float baseMovementSpeed = 0;

    TileMap mazeMap;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();  // Apply MazeCharacter's transform
        target.draw(*sprite, states);
    };
};

#endif
