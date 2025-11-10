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

sf::Vector2f MazeMap::getTargetTileCenter(sf::Vector2i tileCoords) const {
    float halfTile = tileSize / 2.0f;
    return sf::Vector2f(tileCoords.x * tileSize + halfTile,
                        tileCoords.y * tileSize + halfTile);
};

bool MazeMap::isEntityCentered(const Entity& entity) const {
    sf::Vector2f pos = entity.getPosition();
    float halfTile = tileSize / 2.0f;

    sf::Vector2i tileCoords = const_cast<MazeMap*>(this)->getTileCoords(pos);
    sf::Vector2f tileCenter(tileCoords.x * tileSize + halfTile,
                             tileCoords.y * tileSize + halfTile);

    float dx = std::abs(pos.x - tileCenter.x);
    float dy = std::abs(pos.y - tileCenter.y);

    return dx < 1.0f && dy < 1.0f;
}

void MazeMap::snapEntityToGrid(Entity& entity) {
    sf::Vector2f pos = entity.getPosition();
    float halfTile = tileSize / 2.0f;

    sf::Vector2i tileCoords = getTileCoords(pos);
    sf::Vector2f tileCenter(tileCoords.x * tileSize + halfTile,
                             tileCoords.y * tileSize + halfTile);

    entity.setPosition(tileCenter);
}

bool MazeMap::entityCanMove(Entity& entity, MovementDir dir) {
    sf::Vector2f currentPos = entity.getPosition();
    sf::Vector2i currentTile = getTileCoords(currentPos);

    if (dir == MovementDir::UP || dir == MovementDir::DOWN) {
        float halfTile = tileSize / 2.0f;
        float tileCenterX = currentTile.x * tileSize + halfTile;
        if (std::abs(currentPos.x - tileCenterX) > 1.0f) {
            return false;
        }
    } else if (dir == MovementDir::LEFT || dir == MovementDir::RIGHT) {
        float halfTile = tileSize / 2.0f;
        float tileCenterY = currentTile.y * tileSize + halfTile;
        if (std::abs(currentPos.y - tileCenterY) > 1.0f) {
            return false;
        }
    }

    sf::Vector2i targetTile = currentTile;

    switch (dir) {
        case MovementDir::UP:
            targetTile.y -= 1;
            break;
        case MovementDir::DOWN:
            targetTile.y += 1;
            break;
        case MovementDir::LEFT:
            targetTile.x -= 1;
            break;
        case MovementDir::RIGHT:
            targetTile.x += 1;
            break;
        case MovementDir::STATIC:
            return true;
    };

    return !isWall(targetTile.x, targetTile.y);
};
