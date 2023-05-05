/**
 * MapViewStateHelper.h / MapViewStateHelper.cpp
 * 
 * These are files that are used exclusively by the MapViewState class. They are here
 * in an effort to reduce the size/complexity of the MapViewState object as most of these
 * functions do not require access to internal parts of the MapViewState class (and if
 * they do, require use of a specific object).
 */

#pragma once

#include "../Common.h"

#include "../Map/MapCoordinate.h"

#include <map>


namespace NAS2D
{
	namespace Xml
	{
		class XmlElement;
	}
}

class Tile;
class TileMap;
class Warehouse;
class RobotCommand;
class RobotPool;
class Robot;
struct StorableResources;

extern const NAS2D::Point<int> CcNotPlaced;
NAS2D::Point<int>& ccLocation();

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
RobotCommand* getAvailableRobotCommand();

bool simulateMoveProducts(Warehouse*);
void moveProducts(Warehouse*);

void resourceShortageMessage(const StorableResources&, StructureID);

StorableResources addRefinedResources(StorableResources);
void removeRefinedResources(StorableResources&);

void resetTileIndexFromDozer(Robot* robot, Tile* tile);

void updateRobotControl(RobotPool& robotPool);
