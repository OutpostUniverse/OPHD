/**
 * MapViewStateHelper.h / MapViewStateHelper.cpp
 *
 * These are files that are used exclusively by the MapViewState class. They are here
 * in an effort to reduce the size/complexity of the MapViewState object as most of these
 * functions do not require access to internal parts of the MapViewState class (and if
 * they do, require use of a specific object).
 */

#include "MapViewStateHelper.h"

#include "../Constants/Strings.h"
#include "../StructureCatalogue.h"
#include "../StructureManager.h"
#include "../DirectionOffset.h"
#include "../Map/TileMap.h"
#include "../MapObjects/StructureType.h"
#include "../MapObjects/Structures/Warehouse.h"

#include "../UI/MessageBox.h"

#include <NAS2D/Utility.h>

#include <algorithm>


const NAS2D::Point<int> CcNotPlaced{-1, -1};
static NAS2D::Point<int> commandCenterLocation = CcNotPlaced;


constexpr std::array AllDirections4{
	Direction::North,
	Direction::East,
	Direction::South,
	Direction::West,
};


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
	const auto connectorDirection = structure->connectorDirection();

	if (sourceConnectorDir == ConnectorDir::CONNECTOR_INTERSECTION)
	{
		return (connectorDirection == ConnectorDir::CONNECTOR_INTERSECTION || connectorDirection == ConnectorDir::CONNECTOR_VERTICAL) ||
			((dir == Direction::East || dir == Direction::West) ?
				(connectorDirection == ConnectorDir::CONNECTOR_RIGHT) :  // East/West
				(connectorDirection == ConnectorDir::CONNECTOR_LEFT));  // North/South
	}
	else if (sourceConnectorDir == ConnectorDir::CONNECTOR_RIGHT && (dir == Direction::East || dir == Direction::West))
	{
		return (connectorDirection == ConnectorDir::CONNECTOR_INTERSECTION || connectorDirection == ConnectorDir::CONNECTOR_RIGHT || connectorDirection == ConnectorDir::CONNECTOR_VERTICAL);
	}
	else if (sourceConnectorDir == ConnectorDir::CONNECTOR_LEFT && (dir == Direction::North || dir == Direction::South))
	{
		return (connectorDirection == ConnectorDir::CONNECTOR_INTERSECTION || connectorDirection == ConnectorDir::CONNECTOR_LEFT || connectorDirection == ConnectorDir::CONNECTOR_VERTICAL);
	}

	return false;
}


/**
 * Checks to see if the given tile offers a proper connection for a Structure.
 */
bool checkStructurePlacement(Tile& tile, Direction dir)
{
	Structure* structure = tile.structure();
	if (tile.mine() || !tile.bulldozed() || !tile.excavated() || !tile.thingIsStructure() || !structure->connected() || !structure->isConnector())
	{
		return false;
	}

	const auto connectorDirection = structure->connectorDirection();
	return (connectorDirection == ConnectorDir::CONNECTOR_INTERSECTION || connectorDirection == ConnectorDir::CONNECTOR_VERTICAL) ||
		((dir == Direction::East || dir == Direction::West) ?
			(connectorDirection == ConnectorDir::CONNECTOR_RIGHT) :  // East/West
			(connectorDirection == ConnectorDir::CONNECTOR_LEFT));  // North/South
}


/**
 * Checks to see if a tile is a valid tile to place a tube onto.
 */
bool validTubeConnection(TileMap& tilemap, MapCoordinate position, ConnectorDir dir)
{
	return std::any_of(AllDirections4.begin(), AllDirections4.end(), [&](Direction direction){
		return checkTubeConnection(tilemap.getTile(position.translate(direction)), direction, dir);
	});
}


/**
 * Checks a tile to see if a valid Tube connection is available for Structure placement.
 */
bool validStructurePlacement(TileMap& tilemap, MapCoordinate position)
{
	return std::any_of(AllDirections4.begin(), AllDirections4.end(), [&](Direction direction){
		return checkStructurePlacement(tilemap.getTile(position.translate(direction)), direction);
	});
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
bool landingSiteSuitable(TileMap& tilemap, NAS2D::Point<int> position)
{
	for (const auto& offset : DirectionScan3x3)
	{
		auto& tile = tilemap.getTile({position + offset, 0});

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
			throw std::runtime_error("Tile obstructed by a MapObject other than a Mine.");
		}
	}

	return true;
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
	return StructureCatalogue::getType(id).isSelfSustained;
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
StorableResources addRefinedResources(StorableResources resourcesToAdd)
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

	// Return remaining unstored refined resources
	return resourcesToAdd;
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

		auto& resourcesInStorage = structure->storage();
		const auto toTransfer = resourcesToRemove.cap(resourcesInStorage);
		resourcesInStorage -= toTransfer;
		resourcesToRemove -= toTransfer;
	}
}
