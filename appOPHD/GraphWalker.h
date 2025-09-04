#pragma once

#include <vector>


enum class ConnectorDir;
enum class Direction;
struct MapCoordinate;
class TileMap;


void walkGraph(const std::vector<MapCoordinate>& positions, TileMap& tileMap);
void walkGraph(const MapCoordinate& position, TileMap& tileMap);
