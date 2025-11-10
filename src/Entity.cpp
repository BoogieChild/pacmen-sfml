#include "Entity.h"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>

void Entity::setAnimationTiles(sf::Texture& textureSheet, 
                               sf::Vector2i pixelLocation, 
                               const std::string& animationName, 
                               sf::Vector2i tileSize, 
                               unsigned int animationTiles, 
                               unsigned int pixelGap) {
    this->tileSize = tileSize;

    EntityAnimation entityAnimation;
    entityAnimation.name = animationName;

    for (int i = 0; i < animationTiles; i++) {
        sf::Vector2i tilePixelLocation(pixelLocation.x + (i * (pixelGap + tileSize.x)), pixelLocation.y);
        sf::IntRect boundingRect(tilePixelLocation, tileSize);
        sf::Sprite sprite(textureSheet, boundingRect);

        entityAnimation.sprites.push_back(sprite);
    }

    animations.push_back(entityAnimation);
};

void Entity::setActiveSprite(const std::string& animationName, int animationTile) {
    for (auto& animation : animations) {
         if (animation.name == animationName) {
            activeSprite = &animation.sprites.at(animationTile);
            return;
        }
    }
};

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    if (activeSprite) {
        target.draw(*activeSprite, states);
    }
}

void Entity::startMove(MovementDir dir, sf::Vector2f target) {
    currentDirection = dir;
    targetPosition = target;
    isMoving = true;
}

void Entity::update() {
    if (!isMoving || !targetPosition.has_value()) {
        return;
    }

    sf::Vector2f currentPos = getPosition();
    sf::Vector2f target = targetPosition.value();

    sf::Vector2f direction = target - currentPos;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance < 1.0f) {
        setPosition(target);
        isMoving = false;
        targetPosition = std::nullopt;
        return;
    }

    sf::Vector2f movement(0.0f, 0.0f);
    switch (currentDirection) {
        case MovementDir::UP:
            movement.y = -movementSpeed.y;
            if (currentPos.y + movement.y < target.y) {
                movement.y = target.y - currentPos.y;
            }
            break;
        case MovementDir::DOWN:
            movement.y = movementSpeed.y;
            if (currentPos.y + movement.y > target.y) {
                movement.y = target.y - currentPos.y;
            }
            break;
        case MovementDir::LEFT:
            movement.x = -movementSpeed.x;
            if (currentPos.x + movement.x < target.x) {
                movement.x = target.x - currentPos.x;
            }
            break;
        case MovementDir::RIGHT:
            movement.x = movementSpeed.x;
            if (currentPos.x + movement.x > target.x) {
                movement.x = target.x - currentPos.x;
            }
            break;
        case MovementDir::STATIC:
            break;
    }

    Transformable::move(movement);
};
