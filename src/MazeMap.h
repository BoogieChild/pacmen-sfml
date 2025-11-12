#ifndef MAZEMAP_H
#define MAZEMAP_H

#include <SFML/Graphics.hpp>

#include <SFML/System/Vector2.hpp>
#include <vector>
#include <filesystem>
#include "Entity.h"

enum class PelletType {
    NONE,
    DOT,
    ENERGIZER
};

class MazeMap : public sf::Drawable, public sf::Transformable {
public:
    MazeMap() : width(28), height(31), tileSize(8), texture(nullptr), baseMazeSprite(std::nullopt) {}

    bool loadMaze(const std::vector<int>& collisionData,
                  const std::vector<int>& pelletData,
                  sf::Texture& sharedTexture,
                  unsigned int tileSize,
                  sf::Vector2u baseMazeTexturePos,
                  sf::Vector2u pelletMazeTexturePos);

    void eatPellet(int x, int y);

    bool hasPellet(int x, int y) const;

    PelletType getPelletType(sf::Vector2i tilePos) const;

    bool isWall(int x, int y) const;

    int convert2DCoords(int x, int y) const { return x + y * width; }
    bool isLegalTile(int x, int y) const;

    bool entityCanMove(Entity& entity, MovementDir dir);

    sf::Vector2f getTargetTileCenter(sf::Vector2i tileCoords) const;

    sf::Vector2i getTileCoords(sf::Vector2f screenPos);

    bool isEntityCentered(const Entity& entity) const;

    void snapEntityToGrid(Entity& entity);

    void handleTunnelWrapping(Entity& entity);

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    unsigned int getTileSize() const { return tileSize; }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::vector<int> collisionMap;  // 1=wall, 0=path
    std::vector<int> pelletMap;     // 0=none, 1=pellet, 2=power pellet
    std::vector<bool> pelletEaten;

    std::optional<sf::Sprite> baseMazeSprite;
    sf::VertexArray pelletVertices;
    sf::Texture* texture;

    unsigned int width;
    unsigned int height;
    unsigned int tileSize;

    sf::Vector2u baseMazeTexPos;
    sf::Vector2u pelletMazeTexPos;
};

#endif
