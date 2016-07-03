#pragma once

#include "../Common.h"
#include "../StructureManager.h"
#include "../Tile.h"

typedef map<Robot*, Tile*> RobotTileTable;

/**
 * Utility function to cleanly draw a semi-formatted string with an integer value.
 */
void drawString(Renderer& r, Font& f, std::string s, int i, int x, int y, int red = 0, int green = 0, int blue = 0);

/**
 * Utility function to cleanly draw an integer value;
 */
void drawNumber(Renderer& r, Font& f, int i, int x, int y, int red = 0, int green = 0, int blue = 0);

/**
 * Checks to see if a given tube connection is valid.
 */
bool checkTubeConnection(Tile* tile, Direction dir, StructureID type);


/**
 * Checks to see if the given tile offers the a proper connection for a Structure.
 */
bool checkStructurePlacement(Tile *tile, Direction dir);


int totalStorage(StructureManager::StructureList& _sl);


bool insertRobotIntoTable(RobotTileTable& _rm, Robot* robot, Tile* tile);

// Serialize / Deserialize
void checkRobotDeployment(TiXmlElement* _ti, RobotTileTable& _rm, Robot* _r, RobotType _type);
void writeRobots(TiXmlElement* _ti, RobotPool& _rp, RobotTileTable& _rm);
void writeResources(TiXmlElement* _ti, ResourcePool& _rp);