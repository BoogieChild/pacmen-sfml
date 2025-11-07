#ifndef ENTITY_H
#define ENTITY_H

#include "TileMap.h"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <filesystem>

enum class MovementDir {
    STATIC,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Entity : public sf::Drawable, public sf::Transformable {
public:
    Entity(TileMap& map) : map(map) {};

    bool loadTexture(const std::filesystem::path& texturePath, sf::Vector2u graphicTileSize, int tiles);

    void setTextureFrame(const sf::IntRect& rectangle);
protected:
    TileMap map;

    sf::VertexArray vertexArray;
    sf::Texture texture;

    int animationTiles;
    int movementSpeed = 5;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif
