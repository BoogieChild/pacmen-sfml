#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
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
    Entity();

    //assumes entity texture is one row of various animation texture states
    bool loadTexture(const std::filesystem::path& texturePath, sf::Vector2u tileSize, int tiles);
protected:
    sf::VertexArray vertexArray;
    sf::Texture texture;

    int movementSpeed = 5;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif
