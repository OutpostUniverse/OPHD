#pragma once

#include <vector>


struct MapCoordinate;
class Tile;
class TileMap;


std::vector<Tile*> walkGraph(const MapCoordinate& position, TileMap& tileMap);
void walkGraph(const MapCoordinate& position, TileMap& tileMap, std::vector<Tile*>& tileList);
