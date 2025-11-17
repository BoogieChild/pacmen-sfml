#include "Ghost.h"
#include "MazeMap.h"
#include "Pacman.h"
#include <cmath>
#include <SFML/System/Clock.hpp>

static MovementDir oppositeDirection(MovementDir d) {
	switch (d) {
		case MovementDir::UP: return MovementDir::DOWN;
		case MovementDir::DOWN: return MovementDir::UP;
		case MovementDir::LEFT: return MovementDir::RIGHT;
		case MovementDir::RIGHT: return MovementDir::LEFT;
		case MovementDir::STATIC: return MovementDir::STATIC;
	}
	return MovementDir::STATIC;
}

	void Ghost::updateAI(MazeMap& map, const Pacman& pacman) {
	// Only decide a new move when the ghost is not currently moving
	if (isCurrentlyMoving()) return;

	sf::Vector2i currentTile = map.getTileCoords(getPosition());

	sf::Vector2i targetTile;
	
	// If ghost hasn't exited the box yet, force it to target the exit
	if (!hasExitedBox) {
		targetTile = boxExitTile;
		// Check if we've reached or passed the exit
		if (currentTile.y <= boxBoundaryY) {
			hasExitedBox = true;
		}
	} else {
		// Prevent re-entry into box
		if (currentTile.y > boxBoundaryY) {
			// Force target back toward exit
			targetTile = boxExitTile;
		} else if (currentMode == Mode::VULNERABLE) {
			// In vulnerable mode, run away from Pacman
			sf::Vector2i pacmanTile = map.getTileCoords(pacman.getPosition());
			// Target a tile far away from Pacman (simple implementation: go to opposite corner)
			if (pacmanTile.x < map.getWidth() / 2) {
				targetTile.x = map.getWidth() - 1;  // Far right
			} else {
				targetTile.x = 0;  // Far left
			}
			if (pacmanTile.y < map.getHeight() / 2) {
				targetTile.y = map.getHeight() - 1;  // Far bottom
			} else {
				targetTile.y = 0;  // Far top
			}
		} else if (currentMode == Mode::CHASE) {
			// Normal chase/scatter AI
			sf::Vector2i pacmanTile = map.getTileCoords(pacman.getPosition());
			
			if (aiType == AIType::BLINKY) {
				// Blinky: direct chase to Pacman
				targetTile = pacmanTile;
			} else if (aiType == AIType::PINKY) {
				// Pinky: ambush 4 tiles ahead of Pacman
				MovementDir pacmanDir = pacman.getCurrentDirection();
				targetTile = pacmanTile;
				
				// Look 4 tiles ahead in Pacman's direction of travel
				for (int i = 0; i < 4; i++) {
					switch (pacmanDir) {
						case MovementDir::UP:
							targetTile.y -= 1;
							if (targetTile.y < 0) targetTile.y = static_cast<int>(map.getHeight()) - 1;
							break;
						case MovementDir::DOWN:
							targetTile.y += 1;
							if (targetTile.y >= static_cast<int>(map.getHeight())) targetTile.y = 0;
							break;
						case MovementDir::LEFT:
							targetTile.x -= 1;
							if (targetTile.x < 0) targetTile.x = static_cast<int>(map.getWidth()) - 1;
							break;
						case MovementDir::RIGHT:
							targetTile.x += 1;
							if (targetTile.x >= static_cast<int>(map.getWidth())) targetTile.x = 0;
							break;
						case MovementDir::STATIC:
							break;
					}
				}
			} else if (aiType == AIType::INKY) {
				// Inky: targets 2 tiles ahead of Pacman
				MovementDir pacmanDir = pacman.getCurrentDirection();
				targetTile = pacmanTile;
				
				// Look 2 tiles ahead in Pacman's direction of travel
				for (int i = 0; i < 2; i++) {
					switch (pacmanDir) {
						case MovementDir::UP:
							targetTile.y -= 1;
							if (targetTile.y < 0) targetTile.y = static_cast<int>(map.getHeight()) - 1;
							break;
						case MovementDir::DOWN:
							targetTile.y += 1;
							if (targetTile.y >= static_cast<int>(map.getHeight())) targetTile.y = 0;
							break;
						case MovementDir::LEFT:
							targetTile.x -= 1;
							if (targetTile.x < 0) targetTile.x = static_cast<int>(map.getWidth()) - 1;
							break;
						case MovementDir::RIGHT:
							targetTile.x += 1;
							if (targetTile.x >= static_cast<int>(map.getWidth())) targetTile.x = 0;
							break;
						case MovementDir::STATIC:
							break;
					}
				}
			} else {
				// Clyde: direct chase to Pacman
				targetTile = pacmanTile;
			}
		} else {
			// Scatter: each ghost goes to their corner
			if (aiType == AIType::BLINKY) {
				// Blinky: top-right corner
				targetTile = {static_cast<int>(map.getWidth()) - 1, 0};
			} else if (aiType == AIType::PINKY) {
				// Pinky: top-left corner
				targetTile = {0, 0};
			} else if (aiType == AIType::INKY) {
				// Inky: bottom-right corner
				targetTile = {static_cast<int>(map.getWidth()) - 1, static_cast<int>(map.getHeight()) - 1};
			} else {
				// Clyde: bottom-left corner
				targetTile = {0, static_cast<int>(map.getHeight()) - 1};
			}
		}
	}

	int dx = targetTile.x - currentTile.x;
	int dy = targetTile.y - currentTile.y;

	MovementDir primary, secondary;
	if (std::abs(dx) >= std::abs(dy)) {
		primary = dx > 0 ? MovementDir::RIGHT : MovementDir::LEFT;
		secondary = dy > 0 ? MovementDir::DOWN : MovementDir::UP;
	} else {
		primary = dy > 0 ? MovementDir::DOWN : MovementDir::UP;
		secondary = dx > 0 ? MovementDir::RIGHT : MovementDir::LEFT;
	}

	// Build try order. If we're on the box exit tile (or the tile directly below it)
	// prefer moving away from the box to avoid circling: prefer UP first, then LEFT/RIGHT.
	bool onExitOrBelow = (currentTile == boxExitTile) || (currentTile == sf::Vector2i(boxExitTile.x, boxExitTile.y + 1));
	MovementDir tryOrder[4];
	if (onExitOrBelow) {
		tryOrder[0] = MovementDir::UP;
		tryOrder[1] = MovementDir::LEFT;
		tryOrder[2] = MovementDir::RIGHT;
		tryOrder[3] = MovementDir::DOWN;
	} else {
		// Default behaviour: head primarily in the direction closer to target
		tryOrder[0] = primary;
		tryOrder[1] = secondary;
		tryOrder[2] = oppositeDirection(secondary);
		tryOrder[3] = oppositeDirection(primary);
	}
	MovementDir oppositeOfLast = oppositeDirection(lastDirection);

	for (auto dir : tryOrder) {
		if (dir == MovementDir::STATIC) continue;

		// Prevent moving down from the box exit tile (don't allow re-entry)
		if (currentTile == boxExitTile && dir == MovementDir::DOWN) {
			continue;
		}

		// Skip reverse move unless we just changed modes or it's the first move
		if (!allowReversal && dir == oppositeOfLast && lastDirection != MovementDir::STATIC) {
			continue;
		}

		if (map.entityCanMove(*this, dir)) {
			sf::Vector2i nextTile = currentTile;
			switch (dir) {
				case MovementDir::UP: nextTile.y -= 1; break;
				case MovementDir::DOWN: nextTile.y += 1; break;
				case MovementDir::LEFT:
					nextTile.x -= 1;
					if (nextTile.x < 0) nextTile.x = static_cast<int>(map.getWidth()) - 1;
					break;
				case MovementDir::RIGHT:
					nextTile.x += 1;
					if (nextTile.x >= static_cast<int>(map.getWidth())) nextTile.x = 0;
					break;
				case MovementDir::STATIC: break;
			}

			// Set appropriate sprite and start the move toward the tile center
			if (isVulnerable) {
				// Use vulnerable sprite regardless of direction
				setActiveSprite("vulnerable", 0);
			} else {
				switch (dir) {
					case MovementDir::UP: setActiveSprite("up_walking", 0); break;
					case MovementDir::DOWN: setActiveSprite("down_walking", 0); break;
					case MovementDir::LEFT: setActiveSprite("left_walking", 0); break;
					case MovementDir::RIGHT: setActiveSprite("right_walking", 0); break;
					default: break;
				}
			}

			lastDirection = dir;
			allowReversal = false;  // Reset reversal flag after making a move
			sf::Vector2f center = map.getTargetTileCenter(nextTile);
			startMove(dir, center);
			return;
		}
	}
}

void Ghost::setVulnerable(bool vulnerable) {
	if (vulnerable && !isVulnerable) {
		// Entering vulnerable mode
		previousMode = currentMode;  // Save current mode
		currentMode = Mode::VULNERABLE;
		isVulnerable = true;
		allowReversal = true;  // Allow direction reversal when becoming vulnerable
	} else if (!vulnerable && isVulnerable) {
		// Exiting vulnerable mode
		isVulnerable = false;
		currentMode = previousMode;  // Restore previous mode
		allowReversal = true;  // Allow direction reversal when exiting vulnerable
	}
}

void Ghost::endVulnerable() {
	setVulnerable(false);
}

