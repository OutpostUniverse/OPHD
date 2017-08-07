#pragma once

#include "../Common.h"
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
