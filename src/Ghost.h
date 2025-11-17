#ifndef GHOST_H
#define GHOST_H

#include "Entity.h"

class MazeMap;
class Pacman;

class Ghost : public Entity {
public:
	enum class Mode {
		SCATTER,
		CHASE,
		VULNERABLE
	};

	enum class AIType {
		BLINKY,   // Red: direct chase
		PINKY,    // Pink: ambush (4 tiles ahead)
		INKY,     // Blue: complex targeting (placeholder for now)
		CLYDE     // Orange: placeholder
	};

	Ghost(AIType type = AIType::BLINKY) 
		: currentMode(Mode::SCATTER), lastDirection(MovementDir::STATIC), allowReversal(false), aiType(type), hasExitedBox(type == AIType::BLINKY), isVulnerable(false), previousMode(Mode::SCATTER) {}

	void setMode(Mode m) { 
		currentMode = m;
		allowReversal = true;  // Allow reversal when mode changes
	}
	Mode getMode() const { return currentMode; }

	// Vulnerable mode methods
	void setVulnerable(bool vulnerable);
	bool getIsVulnerable() const { return isVulnerable; }
	void endVulnerable();

	// Set the exit tile for the ghost box (where ghost should exit toward)
	void setBoxExitTile(sf::Vector2i exitTile) { boxExitTile = exitTile; }
	
	// Set the box boundary (ghosts can't re-enter beyond this Y coordinate)
	void setBoxBoundaryY(int boundaryY) { boxBoundaryY = boundaryY; }

	// AI step for ghosts with type-specific targeting logic
	void updateAI(MazeMap& map, const Pacman& pacman);

private:
	Mode currentMode;
	MovementDir lastDirection;
	bool allowReversal;
	AIType aiType;
	bool hasExitedBox;
	sf::Vector2i boxExitTile;
	int boxBoundaryY;
	bool isVulnerable;
	Mode previousMode;  // Mode to return to after vulnerable ends
};

#endif
