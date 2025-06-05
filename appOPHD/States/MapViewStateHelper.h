/**
 * MapViewStateHelper.h / MapViewStateHelper.cpp
 *
 * These are files that are used exclusively by the MapViewState class. They are here
 * in an effort to reduce the size/complexity of the MapViewState object as most of these
 * functions do not require access to internal parts of the MapViewState class (and if
 * they do, require use of a specific object).
 */

#pragma once

#include <libOPHD/EnumConnectorDir.h>
#include <libOPHD/EnumProductType.h>
#include <libOPHD/EnumStructureID.h>

#include <libOPHD/Map/MapCoordinate.h>

#include <map>


class Tile;
class TileMap;
class CommandCenter;
class Warehouse;
struct StorableResources;
enum class Direction;

extern const NAS2D::Point<int> CcNotPlaced;
NAS2D::Point<int> ccLocation();
void ccLocation(NAS2D::Point<int> position);

bool isCcPlaced();
bool isPointInCcRange(NAS2D::Point<int> position, int range);
CommandCenter& firstCc();

bool checkTubeConnection(Tile& tile, Direction dir, ConnectorDir sourceConnectorDir);
bool checkStructurePlacement(Tile& tile, Direction dir);
bool validTubeConnection(TileMap& tilemap, MapCoordinate position, ConnectorDir dir);
bool validStructurePlacement(TileMap& tilemap, MapCoordinate position);
bool validLanderSite(Tile& t);
bool landingSiteSuitable(TileMap& tilemap, NAS2D::Point<int> position);
bool structureIsLander(StructureID id);
bool inCommRange(NAS2D::Point<int> position);
bool isPointInRange(NAS2D::Point<int> point1, NAS2D::Point<int> point2, int distance);
bool selfSustained(StructureID id);

Warehouse* getAvailableWarehouse(ProductType type, std::size_t count);

bool simulateMoveProducts(Warehouse*);
void moveProducts(Warehouse*);

void resourceShortageMessage(const StorableResources&, StructureID);

StorableResources addRefinedResources(StorableResources);
void removeRefinedResources(StorableResources&);
