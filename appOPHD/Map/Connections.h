#pragma once

#include <string>


enum class ConnectorDir;
struct MapCoordinate;
class TileMap;
class Road;


ConnectorDir roadConnectorDir(const TileMap& tileMap, const MapCoordinate& mapCoordinate);
std::string roadAnimationName(const Road& road, const TileMap& tileMap);
