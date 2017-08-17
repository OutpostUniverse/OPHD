#pragma once

#include "../Common.h"
#include "../RobotPool.h"
#include "../StructureManager.h"
#include "../Map/TileMap.h"

typedef std::map<Robot*, Tile*> RobotTileTable;

/**
 * Checks to see if a given tube connection is valid.
 */
bool checkTubeConnection(Tile* tile, Direction dir, ConnectorDir _source_connector_dir);


/**
 * Checks to see if the given tile offers the a proper connection for a Structure.
 */
bool checkStructurePlacement(Tile *tile, Direction dir);


/**
* Checks to see if a tile is a valid tile to place a tube onto.
*/
bool validTubeConnection(TileMap* tilemap, int x, int y, ConnectorDir _cd);


/**
* Checks a tile to see if a valid Tube connection is available for Structure placement.
*/
bool validStructurePlacement(TileMap* tilemap, int x, int y);


/**
* Check landing site for obstructions such as mining beacons, things
* and impassable terrain.
*/
bool landingSiteSuitable(TileMap* tilemap, int x, int y);


int totalStorage(StructureManager::StructureList& _sl);


bool structureIsLander(StructureID id);
bool outOfCommRange(StructureManager& sm, Point_2d& cc_location, TileMap* tile_map, Tile* current_tile);

/**
* Determines if the structure requires a tube connection or not.
*
* \note	At the moment this is really just a check for comm towers
*			as all other structures that are self contained are not
*			placeable by the user.
*/
bool selfSustained(StructureID id);

// Serialize / Deserialize
void writeRobots(NAS2D::Xml::XmlElement* _ti, RobotPool& _rp, RobotTileTable& _rm);
void writeResources(NAS2D::Xml::XmlElement* _ti, ResourcePool& _rp);

void readResources(NAS2D::Xml::XmlElement* _ti, ResourcePool& _rp);

void updateRobotControl(RobotPool& _rp, StructureManager& _sm);
