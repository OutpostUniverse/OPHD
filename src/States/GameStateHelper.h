#pragma once

#include "../Common.h"
#include "../StructureManager.h"
#include "../Map/Tile.h"

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


bool insertRobotIntoTable(RobotTileTable& _rm, Robot* robot, Tile* tile);

// Serialize / Deserialize
void writeRobots(NAS2D::Xml::XmlElement* _ti, RobotPool& _rp, RobotTileTable& _rm);
void writeResources(NAS2D::Xml::XmlElement* _ti, ResourcePool& _rp);

void readRobots(NAS2D::Xml::XmlElement* _ti, RobotPool& _rp, RobotTileTable& _rm);
void readResources(NAS2D::Xml::XmlElement* _ti, ResourcePool& _rp);
