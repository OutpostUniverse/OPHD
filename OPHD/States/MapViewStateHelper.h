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
#include "../RobotPool.h"
#include "../StructureManager.h"
#include "../Map/TileMap.h"


using RobotTileTable = std::map<Robot*, Tile*>;

class Warehouse; /**< Forward declaration for getAvailableWarehouse() function. */
class RobotCommand; /**< Forward declaration for getAvailableRobotCommand() function. */

extern const NAS2D::Point<int> CcNotPlaced;
NAS2D::Point<int>& ccLocation();

bool checkTubeConnection(Tile* tile, Direction dir, ConnectorDir sourceConnectorDir);
bool checkStructurePlacement(Tile* tile, Direction dir);
bool validTubeConnection(TileMap* tilemap, int x, int y, ConnectorDir dir);
bool validStructurePlacement(TileMap* tilemap, int x, int y);
bool validLanderSite(Tile* t);
bool landingSiteSuitable(TileMap* tilemap, int x, int y);
bool structureIsLander(StructureID id);
bool outOfCommRange(NAS2D::Point<int>& cc_location, TileMap* tile_map, Tile* current_tile);
bool selfSustained(StructureID id);

int totalStorage(StructureList& structures);

Warehouse* getAvailableWarehouse(ProductType type, std::size_t count);
RobotCommand* getAvailableRobotCommand();

bool simulateMoveProducts(Warehouse*);
void moveProducts(Warehouse*);

void resourceShortageMessage(ResourcePool&, StructureID);

// Serialize / Deserialize
void writeRobots(NAS2D::Xml::XmlElement* element, RobotPool& robotPool, RobotTileTable& robotMap);
void writeResources(NAS2D::Xml::XmlElement* element, ResourcePool& resourcePool, const std::string& tagName);

void readResources(NAS2D::Xml::XmlElement* element, ResourcePool& resourcePool);

void updateRobotControl(RobotPool& robotPool);
void deleteRobotsInRCC(Robot* robot, RobotCommand* rcc, RobotPool& robotPool, RobotTileTable& rtt, Tile* tile);
