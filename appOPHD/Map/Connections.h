#pragma once


enum class ConnectorDir;
struct MapCoordinate;
class TileMap;


ConnectorDir roadConnectorDir(const TileMap& tileMap, const MapCoordinate& mapCoordinate);
