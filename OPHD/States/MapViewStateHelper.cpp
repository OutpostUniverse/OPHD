/**
 * MapViewStateHelper.h / MapViewStateHelper.cpp
 * 
 * These are files that are used exclusively by the MapViewState class. They are here
 * in an effort to reduce the size/complexity of the MapViewState object as most of these
 * functions do not require access to internal parts of the MapViewState class (and if
 * they do, require use of a specific object).
 */

#include "MapViewStateHelper.h"

#include "../Constants.h"
#include "../StructureCatalogue.h"
#include "../StructureManager.h"
#include "../DirectionOffset.h"
#include "../RobotPool.h"
#include "../Map/TileMap.h"
#include "../Things/Structures/RobotCommand.h"
#include "../Things/Structures/Warehouse.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Dictionary.h>
#include <NAS2D/ParserHelper.h>

#include <cmath>


const NAS2D::Point<int> CcNotPlaced{-1, -1};
static NAS2D::Point<int> commandCenterLocation = CcNotPlaced;


NAS2D::Point<int>& ccLocation()
{
	return commandCenterLocation;
}


/**
 * Checks to see if a given tube connection is valid.
 */
bool checkTubeConnection(Tile& tile, Direction dir, ConnectorDir sourceConnectorDir)
{
	if (tile.mine() || !tile.bulldozed() || !tile.excavated() || !tile.thingIsStructure())
	{
		return false;
	}

	Structure* structure = tile.structure();

	if (sourceConnectorDir == ConnectorDir::CONNECTOR_INTERSECTION)
	{
		if (dir == Direction::East || dir == Direction::West)
		{
			if (structure->connectorDirection() == ConnectorDir::CONNECTOR_INTERSECTION || structure->connectorDirection() == ConnectorDir::CONNECTOR_RIGHT || structure->connectorDirection() == ConnectorDir::CONNECTOR_VERTICAL)
			{
				return true;
			}
		}
		else // NORTH/SOUTH
		{
			if (structure->connectorDirection() == ConnectorDir::CONNECTOR_INTERSECTION || structure->connectorDirection() == ConnectorDir::CONNECTOR_LEFT || structure->connectorDirection() == ConnectorDir::CONNECTOR_VERTICAL)
			{
				return true;
			}
		}
	}
	else if (sourceConnectorDir == ConnectorDir::CONNECTOR_RIGHT && (dir == Direction::East || dir == Direction::West))
	{
		if (structure->connectorDirection() == ConnectorDir::CONNECTOR_INTERSECTION || structure->connectorDirection() == ConnectorDir::CONNECTOR_RIGHT || structure->connectorDirection() == ConnectorDir::CONNECTOR_VERTICAL)
		{
			return true;
		}
	}
	else if (sourceConnectorDir == ConnectorDir::CONNECTOR_LEFT && (dir == Direction::North || dir == Direction::South))
	{
		if (structure->connectorDirection() == ConnectorDir::CONNECTOR_INTERSECTION || structure->connectorDirection() == ConnectorDir::CONNECTOR_LEFT || structure->connectorDirection() == ConnectorDir::CONNECTOR_VERTICAL)
		{
			return true;
		}
	}

	return false;
}


/**
 * Checks to see if the given tile offers the a proper connection for a Structure.
 */
bool checkStructurePlacement(Tile& tile, Direction dir)
{
	Structure* structure = tile.structure();
	if (tile.mine() || !tile.bulldozed() || !tile.excavated() || !tile.thingIsStructure() || !tile.connected() || !structure->isConnector())
	{
		return false;
	}

	if (dir == Direction::East || dir == Direction::West)
	{
		if (structure->connectorDirection() == ConnectorDir::CONNECTOR_INTERSECTION || structure->connectorDirection() == ConnectorDir::CONNECTOR_RIGHT || structure->connectorDirection() == ConnectorDir::CONNECTOR_VERTICAL)
		{
			return true;
		}
	}
	else // NORTH/SOUTH
	{
		if (structure->connectorDirection() == ConnectorDir::CONNECTOR_INTERSECTION || structure->connectorDirection() == ConnectorDir::CONNECTOR_LEFT || structure->connectorDirection() == ConnectorDir::CONNECTOR_VERTICAL)
		{
			return true;
		}
	}

	return false;
}


/**
 * Checks to see if a tile is a valid tile to place a tube onto.
 * 
 * \warning		Assumes \c tilemap is never nullptr.
 */
bool validTubeConnection(TileMap* tilemap, NAS2D::Point<int> point, ConnectorDir dir)
{
	return checkTubeConnection(tilemap->getTile(point + DirectionEast, tilemap->currentDepth()), Direction::East, dir) ||
		checkTubeConnection(tilemap->getTile(point + DirectionWest, tilemap->currentDepth()), Direction::West, dir) ||
		checkTubeConnection(tilemap->getTile(point + DirectionSouth, tilemap->currentDepth()), Direction::South, dir) ||
		checkTubeConnection(tilemap->getTile(point + DirectionNorth, tilemap->currentDepth()), Direction::North, dir);
}


/**
 * Checks a tile to see if a valid Tube connection is available for Structure placement.
 *
 * \warning		Assumes \c tilemap is never nullptr.
 */
bool validStructurePlacement(TileMap* tilemap, NAS2D::Point<int> point)
{
	return checkStructurePlacement(tilemap->getTile(point + DirectionNorth, tilemap->currentDepth()), Direction::North) ||
		checkStructurePlacement(tilemap->getTile(point + DirectionEast, tilemap->currentDepth()), Direction::East) ||
		checkStructurePlacement(tilemap->getTile(point + DirectionSouth, tilemap->currentDepth()), Direction::South) ||
		checkStructurePlacement(tilemap->getTile(point + DirectionWest, tilemap->currentDepth()), Direction::West);
}


/**
 * Indicates that the selected landing site is clear of obstructions.
 */
bool validLanderSite(Tile& tile)
{
	if (!tile.empty())
	{
		doAlertMessage(constants::AlertLanderLocation, constants::AlertLanderTileObstructed);
		return false;
	}

	if (!isPointInRange(tile.xy(), ccLocation(), constants::LanderCommRange))
	{
		doAlertMessage(constants::AlertLanderLocation, constants::AlertLanderCommRange);
		return false;
	}

	if (tile.index() == TerrainType::Impassable)
	{
		doAlertMessage(constants::AlertLanderLocation, constants::AlertLanderTerrain);
		return false;
	}

	return true;
}


/**
 * Check landing site for obstructions such as mining beacons, things
 * and impassable terrain.
 * 
 * This is used for the SEED Lander only.
 * 
 * \note	This function will trigger modal dialog boxes to alert
 *			the user as to why the landing site isn't suitable.
 */
bool landingSiteSuitable(TileMap* tilemap, NAS2D::Point<int> position)
{
	for (const auto& offset : DirectionScan3x3)
	{
		auto& tile = tilemap->getTile(position + offset);

		if (tile.index() == TerrainType::Impassable)
		{
			doAlertMessage(constants::AlertLanderLocation, constants::AlertSeedTerrain);
			return false;
		}
		else if (tile.mine())
		{
			doAlertMessage(constants::AlertLanderLocation, constants::AlertSeedMine);
			return false;
		}
		else if (tile.thing())
		{
			// This is a case that should never happen. If it does, blow up loudly.
			throw std::runtime_error("Tile obstructed by a Thing other than a Mine.");
		}
	}

	return true;
}


void deleteRobotsInRCC(Robot* robotToDelete, RobotCommand* rcc, RobotPool& robotPool, RobotTileTable& rtt, Tile* /*tile*/)
{
	if (rcc->isControlling(robotToDelete))
	{
		std::cout << "Cannot bulldoze Robot Command Center by a Robot under its command." << std::endl;
		return;
	}

	const RobotList& rl = rcc->robots();

	for (auto robot : rl)
	{
		if (rtt.find(robot) != rtt.end())
		{
			robot->die();
			continue;
		}

		robotPool.erase(robot);
		delete robot;
	}
}


void updateRobotControl(RobotPool& robotPool)
{
	const auto& commandCenters = NAS2D::Utility<StructureManager>::get().getStructures<CommandCenter>();
	const auto& robotCommands = NAS2D::Utility<StructureManager>::get().getStructures<RobotCommand>();

	// 3 for the first command center
	uint32_t maxRobots = 0;
	if (commandCenters.size() > 0) { maxRobots += 3; }
	// the 10 per robot command facility
	for (std::size_t s = 0; s < robotCommands.size(); ++s)
	{
		if (robotCommands[s]->operational()) { maxRobots += 10; }
	}

	robotPool.InitRobotCtrl(maxRobots);
}


/** 
 * Indicates that a given StructureID is a Lander of sorts.
 */
bool structureIsLander(StructureID id)
{
	return id == StructureID::SID_SEED_LANDER || id == StructureID::SID_COLONIST_LANDER || id == StructureID::SID_CARGO_LANDER;
}


/**
 * Determines if the structure is able to operate without a tube connection.
 */
bool selfSustained(StructureID id)
{
	switch (id)
	{
	case StructureID::SID_COMM_TOWER:
	case StructureID::SID_ROAD:
		return true;

	default:
		return false;
	}
}


/** 
 * Indicates that a specified tile is out of communications range (out of range of a CC or Comm Tower).
 */
bool inCommRange(NAS2D::Point<int> position)
{
	auto& structureManager = NAS2D::Utility<StructureManager>::get();

	const auto& seedLanders = structureManager.getStructures<SeedLander>();
	for (auto lander : seedLanders)
	{
		if (!lander->operational()) { continue; }
		if (isPointInRange(position, structureManager.tileFromStructure(lander).xy(), 5)) // \fixme magic number
		{
			return true;
		}
	}

	const auto& command = structureManager.getStructures<CommandCenter>();
	for (auto cc : command)
	{
		if (!cc->operational()) { continue; }

		if (isPointInRange(position, structureManager.tileFromStructure(cc).xy(), cc->getRange()))
		{
			return true;
		}
	}

	const auto& commTowers = structureManager.getStructures<CommTower>();
	for (auto tower : commTowers)
	{
		if (!tower->operational()) { continue; }

		if (isPointInRange(position, structureManager.tileFromStructure(tower).xy(), tower->getRange()))
		{
			return true;
		}
	}

	return false;
}


bool isPointInRange(NAS2D::Point<int> point1, NAS2D::Point<int> point2, int distance)
{
	return (point2 - point1).lengthSquared() <= distance * distance;
}


/**
 * Gets a pointer to a Warehouse structure that has the specified
 * amount of storage available.
 * 
 * \param	type		Product to store. Use value from ProductType enumerator.
 * \param	count		Count of products that need to be stored.
 * 
 * \return	Returns a pointer to a Warehouse structure or \c nullptr if
 *			there are no warehouses available with the required space.
 */
Warehouse* getAvailableWarehouse(ProductType type, std::size_t count)
{
	for (auto warehouse : NAS2D::Utility<StructureManager>::get().getStructures<Warehouse>())
	{
		if (!warehouse->operational())
		{
			continue;
		}

		if (warehouse->products().canStore(type, static_cast<int>(count)))
		{
			return warehouse;
		}
	}

	return nullptr;
}


/**
 * Gets a RobotCommand structure that has available command capacity
 * to house additional robots.
 * 
 * \note	Assumes a check for only one robot at any given time.
 * 
 * \return	Returns a pointer to a RobotCommand structure or \c nullptr if
 *			there are no robot commands available with the required space.
 */
RobotCommand* getAvailableRobotCommand()
{
	for (auto robotCommand : NAS2D::Utility<StructureManager>::get().getStructures<RobotCommand>())
	{
		if (robotCommand->operational() && robotCommand->commandCapacityAvailable())
		{
			return robotCommand;
		}
	}

	return nullptr;
}


/**
 * Simulates moving the products out of a specified warehouse and raises
 * an alert to the user if not all products can be moved out of the
 * warehouse.
 * 
 * \return	True if all products can be moved or if the user selects "yes"
 *			if bulldozing will result in lost products.
 */
bool simulateMoveProducts(Warehouse* sourceWarehouse)
{
	ProductPool sourcePool = sourceWarehouse->products();
	const auto& warehouses = NAS2D::Utility<StructureManager>::get().getStructures<Warehouse>();
	for (auto warehouse : warehouses)
	{
		if (warehouse->operational())
		{
			if (warehouse != sourceWarehouse)
			{
				ProductPool destinationPool = warehouse->products();
				sourcePool.transferAllTo(destinationPool);
				if (sourcePool.empty())
				{
					return true;
				}
			}
		}
	}

	if (sourcePool.empty())
	{
		return true;
	}

	return doYesNoMessage(constants::ProductTransferTitle, constants::ProductTransferMessage);
}


/**
 * Attempts to move all products from a Warehouse into any remaining warehouses.
 */
void moveProducts(Warehouse* sourceWarehouse)
{
	const auto& warehouses = NAS2D::Utility<StructureManager>::get().getStructures<Warehouse>();
	for (auto warehouse : warehouses)
	{
		if (warehouse->operational())
		{
			if (warehouse != sourceWarehouse)
			{
				sourceWarehouse->products().transferAllTo(warehouse->products());
			}
		}
	}
}


/**
 * Displays a message indicating that there are not enough resources to build
 * a structure and what the missing resources are.
 */
void resourceShortageMessage(const StorableResources& resources, StructureID sid)
{
	StorableResources cost = StructureCatalogue::costToBuild(sid);

	StorableResources missing = cost - resources;

	std::string message = constants::AlertStructureInsufficientResources;

	for (size_t i = 0; i < missing.resources.size(); ++i)
	{
		if (missing.resources[i] >= 0) { message += std::to_string(missing.resources[i]) + " " + ResourceNamesRefined[i] + "\n"; }
	}

	doAlertMessage(constants::AlertInvalidStructureAction, message);
}


/**
 * Add refined resources to the players storage structures.
 */
void addRefinedResources(StorableResources& resourcesToAdd)
{
	/**
	 * The Command Center acts as backup storage especially during the beginning of the
	 * game before storage tanks are built. This ensure that the CC is in the storage
	 * structure list and that it's always the first structure in the list.
	 */

	auto& command = NAS2D::Utility<StructureManager>::get().getStructures<CommandCenter>();
	auto& storageTanks = NAS2D::Utility<StructureManager>::get().getStructures<StorageTanks>();

	std::vector<Structure*> storage;
	storage.insert(storage.end(), command.begin(), command.end());
	storage.insert(storage.end(), storageTanks.begin(), storageTanks.end());

	for (auto structure : storage)
	{
		if (resourcesToAdd.isEmpty()) { break; }

		auto& storageTanksResources = structure->storage();

		auto newResources = storageTanksResources + resourcesToAdd;
		auto capped = newResources.cap(structure->storageCapacity() / 4);

		storageTanksResources = capped;
		resourcesToAdd = newResources - capped;
	}
}


/**
 * Remove refined resources from the players storage structures.
 * 
 * \note	Assumes that enough resources are available and has already
 *			been checked.
 */
void removeRefinedResources(StorableResources& resourcesToRemove)
{
	// Command Center is backup storage, we want to pull from it last

	auto& command = NAS2D::Utility<StructureManager>::get().getStructures<CommandCenter>();
	auto& storageTanks = NAS2D::Utility<StructureManager>::get().getStructures<StorageTanks>();

	std::vector<Structure*> storage;
	storage.insert(storage.end(), storageTanks.begin(), storageTanks.end());
	storage.insert(storage.end(), command.begin(), command.end());

	for (auto structure : storage)
	{
		if (resourcesToRemove.isEmpty()) { break; }

		auto& resourcesInStorage = structure->storage().resources;
		for (size_t i = 0; i < resourcesInStorage.size(); ++i)
		{
			const int pulled = pullResource(resourcesInStorage[i], resourcesToRemove.resources[i]);
			resourcesToRemove.resources[i] -= pulled;
		}
	}
}


/**
 * Pull specified amount of resources from a given quantity.
 * 
 * \note	Modifies param \c resource.
 * 
 * \return	Actual amount pulled.
 */
int pullResource(int& resource, int amount)
{
	if (amount <= resource)
	{
		resource -= amount;
		return amount;
	}
	else
	{
		int ret = resource;
		resource = 0;
		return ret;
	}
}


void resetTileIndexFromDozer(Robot* robot, Tile* tile)
{
	Robodozer* dozer = dynamic_cast<Robodozer*>(robot);
	if (dozer)
	{
		tile->index(static_cast<TerrainType>(dozer->tileIndex()));
	}
}



// ==============================================================
// = CONVENIENCE FUNCTIONS FOR WRITING OUT GAME STATE INFORMATION
// ==============================================================

NAS2D::Dictionary robotToDictionary(RobotTileTable& robotTileTable, Robot& robot)
{
	NAS2D::Dictionary dictionary = robot.getDataDict();

	const auto it = robotTileTable.find(&robot);
	if (it != robotTileTable.end())
	{
		const auto& tile = *it->second;
		const auto position = tile.xy();
		dictionary += NAS2D::Dictionary{{
			{"x", position.x},
			{"y", position.y},
			{"depth", tile.depth()},
		}};
	}

	return dictionary;
}


NAS2D::Xml::XmlElement* writeRobots(RobotPool& robotPool, RobotTileTable& robotMap)
{
	auto* robots = new NAS2D::Xml::XmlElement("robots");

	for (auto robot : robotPool.robots())
	{
		auto dictionary = robotToDictionary(robotMap, *robot);
		robots->linkEndChild(NAS2D::dictionaryToAttributes("robot", dictionary));
	}

	return robots;
}
