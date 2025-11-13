#ifndef PINKY_H
#define PINKY_H

#include "Ghost.h"

class Pinky : public Ghost {
public:
    void setTargetTile(Entity& pacman);
};

#endif
