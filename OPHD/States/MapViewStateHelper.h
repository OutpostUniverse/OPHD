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


namespace NAS2D
{
	namespace Xml
	{
		class XmlElement;
	}
}

class Tile;
class TileMap;
class Warehouse; /**< Forward declaration for getAvailableWarehouse() function. */
class RobotCommand; /**< Forward declaration for getAvailableRobotCommand() function. */
class RobotPool;
class Robot;
struct StorableResources;

using RobotTileTable = std::map<Robot*, Tile*>;

extern const NAS2D::Point<int> CcNotPlaced;
NAS2D::Point<int>& ccLocation();

bool checkTubeConnection(Tile& tile, Direction dir, ConnectorDir sourceConnectorDir);
bool checkStructurePlacement(Tile& tile, Direction dir);
bool validTubeConnection(TileMap& tilemap, NAS2D::Point<int> point, ConnectorDir dir);
bool validStructurePlacement(TileMap& tilemap, NAS2D::Point<int> point);
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

void addRefinedResources(StorableResources&);
void removeRefinedResources(StorableResources&);
int pullResource(int& resource, int amount);

void resetTileIndexFromDozer(Robot* robot, Tile* tile);

// Serialize / Deserialize
NAS2D::Xml::XmlElement* writeRobots(RobotPool& robotPool, RobotTileTable& robotMap);

void updateRobotControl(RobotPool& robotPool);
void deleteRobotsInRCC(Robot* robot, RobotCommand* rcc, RobotPool& robotPool, RobotTileTable& rtt, Tile* tile);
