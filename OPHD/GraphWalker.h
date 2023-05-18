#pragma once

#include <vector>


struct MapCoordinate;
class Tile;
class TileMap;


std::vector<Tile*> walkGraph(const std::vector<MapCoordinate>& positions, TileMap& tileMap);
void walkGraph(const MapCoordinate& position, TileMap& tileMap, std::vector<Tile*>& tileList);
