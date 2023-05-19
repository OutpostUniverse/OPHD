#pragma once

#include <vector>


struct MapCoordinate;
class Tile;
class TileMap;


void walkGraph(const std::vector<MapCoordinate>& positions, TileMap& tileMap);
void walkGraph(const MapCoordinate& position, TileMap& tileMap);

void extendWalk(const MapCoordinate& position, TileMap& tileMap);
