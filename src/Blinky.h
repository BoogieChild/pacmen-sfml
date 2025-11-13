#ifndef BLINKY_H
#define BLINKY_H

#include "Ghost.h"

class Blinky : public Ghost {
public:
    void setTargetTile(Entity& pacman);
};

#endif
