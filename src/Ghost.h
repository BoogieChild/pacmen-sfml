#ifndef GHOST_H
#define GHOST_H

#include "Entity.h"
#include <SFML/System/Vector2.hpp>

class Ghost : public Entity {
public:
    virtual void setTargetTile() = 0;
protected:
    sf::Vector2i targetTile;
};

#endif
