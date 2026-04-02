#pragma once

#include "EnumDirection.h"

#include <array>


/**
 * Picks a digger orientation that points away from already excavated neighbors.
 *
 * The neighbor array uses the same ordering as DirectionClockwise8:
 * North, NorthEast, East, SouthEast, South, SouthWest, West, NorthWest.
 */
Direction chooseDiggerDirectionAwayFromExcavated(const std::array<bool, 8>& excavatedNeighbors);

bool hasExcavatedNeighbor(const std::array<bool, 8>& neighbors);

struct AutoDiggerPlacement
{
	bool canAutoPlace = false;
	Direction direction = Direction::North;
};

AutoDiggerPlacement decideAutoDiggerPlacement(bool tileExcavated, const std::array<bool, 8>& excavatedNeighbors);
