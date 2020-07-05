// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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
#include "../DirectionOffset.h"


#include "../Things/Structures/RobotCommand.h"
#include "../Things/Structures/Warehouse.h"

#include <cmath>

using namespace NAS2D;
using namespace NAS2D::Xml;


extern int ROBOT_ID_COUNTER; /// \fixme Kludge


const NAS2D::Point<int> CcNotPlaced{-1, -1};
static Point<int> commandCenterLocation = CcNotPlaced;


/**
 * 
 */
Point<int>& ccLocation()
{
	return commandCenterLocation;
}


/**
 * Checks to see if a given tube connection is valid.
 * 
 * \warning		Assumes \c tile is never nullptr.
 */
bool checkTubeConnection(Tile* tile, Direction dir, ConnectorDir sourceConnectorDir)
{
	if (tile->mine() || !tile->bulldozed() || !tile->excavated() || !tile->thingIsStructure())
	{
		return false;
	}

	Structure* structure = tile->structure();

	if (sourceConnectorDir == ConnectorDir::CONNECTOR_INTERSECTION)
	{
		if (dir == Direction::DIR_EAST || dir == Direction::DIR_WEST)
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
	else if (sourceConnectorDir == ConnectorDir::CONNECTOR_RIGHT && (dir == Direction::DIR_EAST || dir == Direction::DIR_WEST))
	{
		if (structure->connectorDirection() == ConnectorDir::CONNECTOR_INTERSECTION || structure->connectorDirection() == ConnectorDir::CONNECTOR_RIGHT || structure->connectorDirection() == ConnectorDir::CONNECTOR_VERTICAL)
		{
			return true;
		}
	}
	else if (sourceConnectorDir == ConnectorDir::CONNECTOR_LEFT && (dir == Direction::DIR_NORTH || dir == Direction::DIR_SOUTH))
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
 * 
 * \warning		Assumes \c tile is never nullptr.
 */
bool checkStructurePlacement(Tile* tile, Direction dir)
{
	if (tile->mine() || !tile->bulldozed() || !tile->excavated() || !tile->thingIsStructure() || !tile->connected())
	{
		return false;
	}

	Structure* _structure = tile->structure();
	if (!_structure->isConnector())
	{
		return false;
	}

	if (dir == Direction::DIR_EAST || dir == Direction::DIR_WEST)
	{
		if (_structure->connectorDirection() == ConnectorDir::CONNECTOR_INTERSECTION || _structure->connectorDirection() == ConnectorDir::CONNECTOR_RIGHT)
		{
			return true;
		}
	}
	else // NORTH/SOUTH
	{
		if (_structure->connectorDirection() == ConnectorDir::CONNECTOR_INTERSECTION || _structure->connectorDirection() == ConnectorDir::CONNECTOR_LEFT)
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
	return checkTubeConnection(tilemap->getTile(point + DirectionEast, tilemap->currentDepth()), Direction::DIR_EAST, dir) ||
		checkTubeConnection(tilemap->getTile(point + DirectionWest, tilemap->currentDepth()), Direction::DIR_WEST, dir) ||
		checkTubeConnection(tilemap->getTile(point + DirectionSouth, tilemap->currentDepth()), Direction::DIR_SOUTH, dir) ||
		checkTubeConnection(tilemap->getTile(point + DirectionNorth, tilemap->currentDepth()), Direction::DIR_NORTH, dir);
}


/**
 * Checks a tile to see if a valid Tube connection is available for Structure placement.
 *
 * \warning		Assumes \c tilemap is never nullptr.
 */
bool validStructurePlacement(TileMap* tilemap, int x, int y)
{
	const auto point = NAS2D::Point{x, y};
	return checkStructurePlacement(tilemap->getTile(point + DirectionNorth, tilemap->currentDepth()), Direction::DIR_NORTH) ||
		checkStructurePlacement(tilemap->getTile(point + DirectionEast, tilemap->currentDepth()), Direction::DIR_EAST) ||
		checkStructurePlacement(tilemap->getTile(point + DirectionSouth, tilemap->currentDepth()), Direction::DIR_SOUTH) ||
		checkStructurePlacement(tilemap->getTile(point + DirectionWest, tilemap->currentDepth()), Direction::DIR_WEST);
}


/**
 * Indicates that the selected landing site is clear of obstructions.
 *
 * \warning		Assumes \c tile is never nullptr.
 */
bool validLanderSite(Tile* tile)
{
	if (!tile->empty())
	{
		doAlertMessage(constants::ALERT_LANDER_LOCATION, constants::ALERT_LANDER_TILE_OBSTRUCTED);
		return false;
	}

	if (tile->distanceTo(ccLocation()) > constants::LANDER_COM_RANGE)
	{
		doAlertMessage(constants::ALERT_LANDER_LOCATION, constants::ALERT_LANDER_COMM_RANGE);
		return false;
	}

	if (tile->index() == TerrainType::TERRAIN_IMPASSABLE)
	{
		doAlertMessage(constants::ALERT_LANDER_LOCATION, constants::ALERT_LANDER_TERRAIN);
		return false;
	}

	return true;
}


/**
 * Document me!
 */
int totalStorage(StructureList& structures)
{
	int storage = 0;
	for (std::size_t i = 0; i < structures.size(); ++i)
	{
		if (structures[i]->operational())
		{
			storage += structures[i]->storage().capacity();
		}
	}

	return constants::BASE_STORAGE_CAPACITY + storage;
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
bool landingSiteSuitable(TileMap* tilemap, int x, int y)
{
	for (int offY = y - 1; offY <= y + 1; ++offY)
	{
		for (int offX = x - 1; offX <= x + 1; ++offX)
		{
			Tile* tile = tilemap->getTile({offX, offY});

			if (tile->index() == TerrainType::TERRAIN_IMPASSABLE)
			{
				doAlertMessage(constants::ALERT_LANDER_LOCATION, constants::ALERT_SEED_TERRAIN);
				return false;
			}
			else if (tile->mine())
			{
				doAlertMessage(constants::ALERT_LANDER_LOCATION, constants::ALERT_SEED_MINE);
				return false;
			}
			else if (tile->thing())
			{
				// This is a case that should never happen. If it does, blow up loudly.
				throw std::runtime_error("Tile obstructed by a Thing other than a Mine.");
			}
		}
	}

	return true;
}


/**
 * Document me!
 */
void deleteRobotsInRCC(Robot* robotToDelete, RobotCommand* rcc, RobotPool& robotPool, RobotTileTable& rtt, Tile* /*tile*/)
{
	if (rcc->commandedByThis(robotToDelete))
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


/**
 * Document me!
 */
void updateRobotControl(RobotPool& robotPool)
{
	const auto& CommandCenter = Utility<StructureManager>::get().structureList(Structure::StructureClass::CLASS_COMMAND);
	const auto& RobotCommand = Utility<StructureManager>::get().structureList(Structure::StructureClass::CLASS_ROBOT_COMMAND);

	// 3 for the first command center
	uint32_t _maxRobots = 0;
	if (CommandCenter.size() > 0) { _maxRobots += 3; }
	// the 10 per robot command facility
	for (std::size_t s = 0; s < RobotCommand.size(); ++s)
	{
		if (RobotCommand[s]->operational()) { _maxRobots += 10; }
	}

	robotPool.InitRobotCtrl(_maxRobots);
}


/** 
 * Indicates that a given StructureID is a Lander of sorts.
 */
bool structureIsLander(StructureID id)
{
	return id == StructureID::SID_SEED_LANDER || id == StructureID::SID_COLONIST_LANDER || id == StructureID::SID_CARGO_LANDER;
}


/**
 * Determines if the structure requires a tube connection or not.
 *
 * \note	At the moment this is really just a check for comm towers
 *			as all other structures that are self contained are not
 *			placeable by the user.
 */
bool selfSustained(StructureID id)
{
	return id == StructureID::SID_COMM_TOWER;
}


/** 
 * Indicates that a specified tile is out of communications range (out of range of a CC or Comm Tower).
 */
bool outOfCommRange(Point<int>& cc_location, TileMap* tile_map, Tile* current_tile)
{
	Tile* tile = tile_map->getVisibleTile();

	if (tile->distanceTo(tile_map->getTile(cc_location, 0)) <= constants::ROBOT_COM_RANGE)
		return false;

	Tile* _comm_t = nullptr;
	for (auto _tower : Utility<StructureManager>::get().structureList(Structure::StructureClass::CLASS_COMM))
	{
		if (!_tower->operational())
		{
			continue;
		}

		_comm_t = Utility<StructureManager>::get().tileFromStructure(_tower);
		if (_comm_t->distanceTo(current_tile) <= constants::COMM_TOWER_BASE_RANGE)
		{
			return false;
		}
	}

	return true;
}


/**
 * Gets a pointer to a Warehouse structure that has the specified
 * amount of storage available.
 * 
 * \param	_pt		Product to store. Use value from ProductType enumerator.
 * \param	_ct		Count of products that need to be stored.
 * 
 * \return	Returns a pointer to a Warehouse structure or \c nullptr if
 *			there are no warehouses available with the required space.
 */
Warehouse* getAvailableWarehouse(ProductType type, std::size_t count)
{
	for (auto structure : Utility<StructureManager>::get().structureList(Structure::StructureClass::CLASS_WAREHOUSE))
	{
		Warehouse* _wh = static_cast<Warehouse*>(structure);
		if (_wh->products().canStore(type, static_cast<int>(count)))
		{
			return _wh;
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
 * \return	Returns a pointer to a Warehouse structure or \c nullptr if
 *			there are no warehouses available with the required space.
 */
RobotCommand* getAvailableRobotCommand()
{
	for (auto structure : Utility<StructureManager>::get().structureList(Structure::StructureClass::CLASS_ROBOT_COMMAND))
	{
		RobotCommand* _rc = static_cast<RobotCommand*>(structure);
		if (_rc->operational() && _rc->commandCapacityAvailable())
		{
			return _rc;
		}
	}

	return nullptr;
}


/**
 * Used in the product move simulation. Very brute force.
 */
void transferProductsPool(ProductPool& source, ProductPool& destination)
{
	if (source.empty() || destination.atCapacity()) { return; }

	auto& src = source.mProducts;

	for (std::size_t i = 0; i < ProductType::PRODUCT_COUNT; ++i)
	{
		if (destination.availableStorage() == 0) { return; }

		if (destination.availableStorage() >= storageRequired(static_cast<ProductType>(i), src[i]))
		{
			destination.store(static_cast<ProductType>(i), src[i]);
			source.pull(static_cast<ProductType>(i), src[i]);
		}
		else
		{
			int units_to_move = destination.availableStorage() / storageRequiredPerUnit(static_cast<ProductType>(i));
			destination.store(static_cast<ProductType>(i), units_to_move);
			source.pull(static_cast<ProductType>(i), units_to_move);
		}
	}
}




/**
 * Simulates moving the products out of a specified warehouse and raises
 * an alert to the user if not all products can be moved out of the
 * warehouse.
 * 
 * \return	True if all products can be moved or if the user selects "yes"
 *			if bulldozing will result in lost products.
 */
bool simulateMoveProducts(Warehouse* wh)
{
	ProductPool _pool = wh->products();

	/** \fixme	This is a brute force approach. It works but it's not elegant. */
	const auto& structures = Utility<StructureManager>::get().structureList(Structure::StructureClass::CLASS_WAREHOUSE);
	for (auto structure : structures)
	{
		if (structure->operational())
		{
			Warehouse* warehouse = static_cast<Warehouse*>(structure);
			if (warehouse != wh)
			{
				ProductPool _tfrPool = warehouse->products();
				transferProductsPool(_pool, _tfrPool);
			}
		}
	}

	if (!_pool.empty()) { return doYesNoMessage(constants::PRODUCT_TRANSFER_TITLE, constants::PRODUCT_TRANSFER_MESSAGE); }

	return true;
}


/**
 * Attempts to move all products from a Warehouse into any remaining warehouses.
 */
void moveProducts(Warehouse* wh)
{
	const auto& structures = Utility<StructureManager>::get().structureList(Structure::StructureClass::CLASS_WAREHOUSE);
	for (auto structure : structures)
	{
		if (structure->operational())
		{
			Warehouse* warehouse = static_cast<Warehouse*>(structure);
			if (warehouse != wh)
			{
				transferProductsStructure(wh, warehouse);
			}
		}
	}
}


/**
 * Displays a message indicating that there are not enough resources to build
 * a structure and what the missing resources are.
 */
void resourceShortageMessage(ResourcePool& resourcePool, StructureID sid)
{
	const ResourcePool& cost = StructureCatalogue::costToBuild(sid);

	ResourcePool missing;
	
	if (resourcePool.commonMetals() < cost.commonMetals()) { missing.commonMetals(cost.commonMetals() - resourcePool.commonMetals()); }
	if (resourcePool.commonMinerals() < cost.commonMinerals()) { missing.commonMinerals(cost.commonMinerals() - resourcePool.commonMinerals()); }
	if (resourcePool.rareMetals() < cost.rareMetals()) { missing.rareMetals(cost.rareMetals() - resourcePool.rareMetals()); }
	if (resourcePool.rareMinerals() < cost.rareMinerals()) { missing.rareMinerals(cost.rareMinerals() - resourcePool.rareMinerals()); }

	std::string message = constants::ALERT_STRUCTURE_INSUFFICIENT_RESORUCES;

	if (missing.commonMetals() != 0) { message += std::to_string(missing.commonMetals()) + " Common Metals" + "\n"; }
	if (missing.commonMinerals() != 0) { message += std::to_string(missing.commonMinerals()) + " Common Minerals" + "\n"; }
	if (missing.rareMetals() != 0) { message += std::to_string(missing.rareMetals()) + " Rare Metals" + "\n"; }
	if (missing.rareMinerals() != 0) { message += std::to_string(missing.rareMinerals()) + " Rare Minerals"; }

	doAlertMessage(constants::ALERT_INVALID_STRUCTURE_ACTION, message);
}



// ==============================================================
// = CONVENIENCE FUNCTIONS FOR WRITING OUT GAME STATE INFORMATION
// ==============================================================

/** 
 * Document me!
 */
void checkRobotDeployment(XmlElement* _ti, RobotTileTable& _rm, Robot* _r, RobotType _type)
{
	_ti->attribute("id", _r->id());
	_ti->attribute("type", _type);
	_ti->attribute("age", _r->fuelCellAge());
	_ti->attribute("production", _r->turnsToCompleteTask());

	const auto it = _rm.find(_r);
	if (it->first == _r)
	{
		_ti->attribute("x", it->second->x());
		_ti->attribute("y", it->second->y());
		_ti->attribute("depth", it->second->depth());
	}

}


/** 
 * Document me!
 * 
 * Convenience function
 */
void writeRobots(NAS2D::Xml::XmlElement* element, RobotPool& robotPool, RobotTileTable& robotMap)
{
	XmlElement* robots = new XmlElement("robots");
	robots->attribute("id_counter", ROBOT_ID_COUNTER);

	RobotPool::DiggerList& diggers = robotPool.diggers();

	for (auto digger : diggers)
	{
		XmlElement* robot = new XmlElement("robot");
		checkRobotDeployment(robot, robotMap, digger, RobotType::ROBOT_DIGGER);
		robot->attribute("direction", digger->direction());
		robots->linkEndChild(robot);
	}

	RobotPool::DozerList& dozers = robotPool.dozers();
	for (auto dozer : dozers)
	{
		XmlElement* robot = new XmlElement("robot");
		checkRobotDeployment(robot, robotMap, dozer, RobotType::ROBOT_DOZER);
		robots->linkEndChild(robot);
	}

	RobotPool::MinerList& miners = robotPool.miners();
	for (auto miner : miners)
	{
		XmlElement* robot = new XmlElement("robot");
		checkRobotDeployment(robot, robotMap, miner, RobotType::ROBOT_MINER);
		robots->linkEndChild(robot);
	}

	element->linkEndChild(robots);
}


/** 
 * Document me!
 */
void writeResources(NAS2D::Xml::XmlElement* element, ResourcePool& resourcePool, const std::string& tagName)
{
	XmlElement* resources = new XmlElement(tagName);
	resourcePool.serialize(resources);
	element->linkEndChild(resources);
}


/** 
 * Document me!
 */
void readResources(NAS2D::Xml::XmlElement* element, ResourcePool& resourcePool)
{
	if (element) { resourcePool.deserialize(element); }
}
