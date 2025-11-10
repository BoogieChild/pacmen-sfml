#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

enum class MovementDir {
    STATIC,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct EntityAnimation {
    std::string name;
    std::vector<sf::Sprite> sprites;
};

class Entity : public sf::Transformable, public sf::Drawable {
public:
    Entity() : movementSpeed({5.0f, 5.0f}),
               currentDirection(MovementDir::STATIC),
               queuedDirection(MovementDir::STATIC),
               targetPosition(std::nullopt),
               isMoving(false) {};

    void setAnimationTiles(sf::Texture& textureSheet, sf::Vector2i pixelLocation, const std::string& animationName, sf::Vector2i tileSize, unsigned int animationTiles, unsigned int pixelGap);

    sf::Sprite* getActiveSprite() { return activeSprite; };

    void setActiveSprite(const std::string& animationName, int animationTile);

    void startMove(MovementDir dir, sf::Vector2f target);

    void update();

    void queueDirection(MovementDir dir) { queuedDirection = dir; }

    MovementDir getQueuedDirection() const { return queuedDirection; }

    void clearQueuedDirection() { queuedDirection = MovementDir::STATIC; }

    bool isCurrentlyMoving() const { return isMoving; }

    MovementDir getCurrentDirection() const { return currentDirection; }

    sf::Vector2f getMovementSpeed() const { return movementSpeed; }

protected:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Vector2i tileSize;

    std::vector<EntityAnimation> animations;

    sf::Sprite* activeSprite;

    sf::Vector2f movementSpeed;

    MovementDir currentDirection;

    MovementDir queuedDirection;

    std::optional<sf::Vector2f> targetPosition;

    bool isMoving;
};

#endif
