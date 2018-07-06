/**
 * GameStateHelper.h / GameStateHelper.cpp
 * 
 * These are files that are used exclusively by the GameState class. They are here
 * in an effort to reduce the size/complexity of the GameState object as most of these
 * functions do not require access to internal parts of the GameState class (and if
 * they do, require use of a specific object).
 */

#pragma once

#include "../Common.h"
#include "../RobotPool.h"
#include "../StructureManager.h"
#include "../Map/TileMap.h"


typedef std::map<Robot*, Tile*> RobotTileTable; /**<  */

class Warehouse;	/**< Forward declaration for getAvailableWarehouse() function. */
class RobotCommand;	/**< Forward declaration for getAvailableRobotCommand() function. */


bool checkTubeConnection(Tile* tile, Direction dir, ConnectorDir _source_connector_dir);
bool checkStructurePlacement(Tile* tile, Direction dir);
bool validTubeConnection(TileMap* tilemap, int x, int y, ConnectorDir _cd);
bool validStructurePlacement(TileMap* tilemap, int x, int y);
bool validLanderSite(Tile* t);
bool landingSiteSuitable(TileMap* tilemap, int x, int y);
bool structureIsLander(StructureID id);
bool outOfCommRange(StructureManager& sm, Point_2d& cc_location, TileMap* tile_map, Tile* current_tile);
bool selfSustained(StructureID id);

int totalStorage(StructureManager::StructureList& _sl);

Warehouse* getAvailableWarehouse(StructureManager& _sm, ProductType _pt, size_t _ct);
RobotCommand* getAvailableRobotCommand(StructureManager& _sm);
void moveProducts(Warehouse* wh, StructureManager& _sm);


// Serialize / Deserialize
void writeRobots(NAS2D::Xml::XmlElement* _ti, RobotPool& _rp, RobotTileTable& _rm);
void writeResources(NAS2D::Xml::XmlElement* _ti, ResourcePool& _rp);

void readResources(NAS2D::Xml::XmlElement* _ti, ResourcePool& _rp);

void updateRobotControl(RobotPool& _rp, StructureManager& _sm);
void deleteRobotsInRCC(Robot* r, RobotCommand* rcc, RobotPool& rp, RobotTileTable& rtt, Tile* tile);
