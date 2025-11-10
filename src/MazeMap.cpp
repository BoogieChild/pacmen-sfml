#include "MazeMap.h"
#include <SFML/System/Vector2.hpp>

bool MazeMap::loadMaze(const std::vector<int>& collisionData,
                       const std::vector<int>& pelletData,
                       sf::Texture& sharedTexture,
                       unsigned int tileSizeParam,
                       sf::Vector2u baseMazeTexturePos,
                       sf::Vector2u pelletMazeTexturePos) {

    this->tileSize = tileSizeParam;
    this->texture = &sharedTexture;
    this->collisionMap = collisionData;
    this->pelletMap = pelletData;
    this->baseMazeTexPos = baseMazeTexturePos;
    this->pelletMazeTexPos = pelletMazeTexturePos;

    pelletEaten.resize(width * height, false);

    baseMazeSprite.emplace(*texture);
    baseMazeSprite->setTextureRect(sf::IntRect(
        sf::Vector2i(baseMazeTexturePos),
        sf::Vector2i(width * tileSize, height * tileSize)
    ));

    pelletVertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    pelletVertices.resize(width * height * 6);

    for (unsigned int x = 0; x < width; ++x) {
        for (unsigned int y = 0; y < height; ++y) {
            int tileIndex = x + y * width;
            int pelletValue = pelletMap[tileIndex];

            sf::Vertex* triangles = &pelletVertices[tileIndex * 6];

            triangles[0].position = sf::Vector2f(x * tileSize, y * tileSize);
            triangles[1].position = sf::Vector2f((x + 1) * tileSize, y * tileSize);
            triangles[2].position = sf::Vector2f(x * tileSize, (y + 1) * tileSize);
            triangles[3].position = sf::Vector2f(x * tileSize, (y + 1) * tileSize);
            triangles[4].position = sf::Vector2f((x + 1) * tileSize, y * tileSize);
            triangles[5].position = sf::Vector2f((x + 1) * tileSize, (y + 1) * tileSize);

            float texX = pelletMazeTexturePos.x + x * tileSize;
            float texY = pelletMazeTexturePos.y + y * tileSize;

            triangles[0].texCoords = sf::Vector2f(texX, texY);
            triangles[1].texCoords = sf::Vector2f(texX + tileSize, texY);
            triangles[2].texCoords = sf::Vector2f(texX, texY + tileSize);
            triangles[3].texCoords = sf::Vector2f(texX, texY + tileSize);
            triangles[4].texCoords = sf::Vector2f(texX + tileSize, texY);
            triangles[5].texCoords = sf::Vector2f(texX + tileSize, texY + tileSize);

            sf::Color color = (pelletValue > 0) ? sf::Color::White : sf::Color::Transparent;
            for (int i = 0; i < 6; ++i) {
                triangles[i].color = color;
            }
        }
    }

    return true;
}

void MazeMap::eatPellet(int x, int y) {
    if (!isLegalTile(x, y)) return;

    int tileIndex = x + y * width;

    pelletEaten[tileIndex] = true;

    sf::Vertex* triangles = &pelletVertices[tileIndex * 6];
    for (int i = 0; i < 6; ++i) {
        triangles[i].color = sf::Color::Transparent;
    }
}

bool MazeMap::hasPellet(int x, int y) const {
    if (!isLegalTile(x, y)) return false;

    int tileIndex = x + y * width;

    return !pelletEaten[tileIndex] && pelletMap[tileIndex] > 0;
}

bool MazeMap::isWall(int x, int y) const {
    if (!isLegalTile(x, y)) return true;

    int tileIndex = x + y * width;
    return collisionMap[tileIndex] == 1;
}

bool MazeMap::isLegalTile(int x, int y) const {
    return x >= 0 && x < static_cast<int>(width) &&
           y >= 0 && y < static_cast<int>(height);
}

void MazeMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!texture || !baseMazeSprite.has_value()) return;

    states.transform *= getTransform();
    states.texture = texture;

    target.draw(*baseMazeSprite, states);

    target.draw(pelletVertices, states);
}

// assumes maze is drawn at 0,0
sf::Vector2i MazeMap::getTileCoords(sf::Vector2f screenPos) {
    int tileX = static_cast<int>(screenPos.x / tileSize);
    int tileY = static_cast<int>(screenPos.y / tileSize);

    return {tileX, tileY};
};

bool MazeMap::entityCanMove(Entity& entity, MovementDir dir) {
    sf::Vector2i coords = getTileCoords(entity.getPosition());

    int xTile = coords.x;
    int yTile = coords.y;

    switch (dir) {
        case MovementDir::UP:
            return !isWall(xTile, yTile - 1);
            break;
        case MovementDir::DOWN:
            return !isWall(xTile, yTile + 1);
            break;
        case MovementDir::LEFT:
            return !isWall(xTile - 1, yTile);
            break;
        case MovementDir::RIGHT:
            return !isWall(xTile + 1, yTile);
            break;
        case MovementDir::STATIC:
            return true;
            break;
    };

    return false;
};
