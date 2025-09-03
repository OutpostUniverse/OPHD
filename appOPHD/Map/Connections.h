#pragma once


enum class ConnectorDir;
struct MapCoordinate;
class TileMap;


ConnectorDir tubeConnectorDir(const TileMap& tileMap, const MapCoordinate& mapCoordinate);
ConnectorDir roadConnectorDir(const TileMap& tileMap, const MapCoordinate& mapCoordinate);
