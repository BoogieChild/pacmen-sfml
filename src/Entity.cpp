#include "Entity.h"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <filesystem>
#include <iostream>

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

void Entity::move(MovementDir dir) {
    switch (dir) {
        case MovementDir::UP:
            Transformable::move({0, -movementSpeed.y});
            break;
        case MovementDir::DOWN:
            Transformable::move({0, movementSpeed.y});
            break;
        case MovementDir::LEFT:
            Transformable::move({-movementSpeed.x, 0});
            break;
        case MovementDir::RIGHT:
            Transformable::move({movementSpeed.x, 0});
            break;
        case MovementDir::STATIC:
            break;
    };
};
