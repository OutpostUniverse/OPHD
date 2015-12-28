#pragma once

#include "../Common.h"
#include "../Tile.h"

/**
 * Utility function to cleanly draw a semi-formatted string with an integer value.
 */
void drawString(Renderer& r, Font& f, std::string s, int i, int x, int y, int red = 0, int green = 0, int blue = 0);

/**
 * Utility function to cleanly draw an integer value;
 */
void drawNumber(Renderer& r, Font& f, int i, int x, int y, int red = 0, int green = 0, int blue = 0);

/**
 * Checks to see if a given tube connection is valid.
 */
bool checkTubeConnection(Tile* tile, Direction dir, StructureType type);


/**
 * Checks to see if the given tile offers the a proper connection for a Structure.
 */
bool checkStructurePlacement(Tile *tile, Direction dir);