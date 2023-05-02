#pragma once

#include "Map/MapCoordinate.h"

#include <vector>


class Tile;
class TileMap;


void walkGraph(const MapCoordinate& position, TileMap& tileMap, std::vector<Tile*>& tileList);
