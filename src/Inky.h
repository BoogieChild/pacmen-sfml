#ifndef INKY_H
#define INKY_H

#include "Ghost.h"

class Inky : public Ghost {
public:
    void setTargetTile(Entity& pacman, Entity& blinky);
};

#endif
