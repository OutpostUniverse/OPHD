#pragma once

#include <vector>


enum class ConnectorDir;
enum class Direction;
struct MapCoordinate;
class TileMap;


bool hasConnectorDirection(ConnectorDir srcConnectorDir, Direction direction);

void walkGraph(const std::vector<MapCoordinate>& positions, TileMap& tileMap);
void walkGraph(const MapCoordinate& position, TileMap& tileMap);
