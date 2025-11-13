#ifndef CLYDE_H
#define CLYDE_H

#include "Ghost.h"

class Clyde : public Ghost {
public:
    void setTargetTile(Entity& pacman);
};

#endif
