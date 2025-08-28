#include "OreHaulRoutes.h"

#include "../Constants/Numbers.h"
#include "Route.h"
#include "Tile.h"
#include "../StructureManager.h"
#include "../MapObjects/Structures/MineFacility.h"

#include <NAS2D/Utility.h>

#include <cfloat>
#include <map>
#include <algorithm>


namespace
{
	auto& getRouteTable()
	{
		return NAS2D::Utility<std::map<const MineFacility*, Route>>::get();
	}
}


OreHaulRoutes::OreHaulRoutes(TileMap& tileMap, const StructureManager& structureManager) :
	mRouteFinder{tileMap},
	mStructureManager{structureManager}
{
}


bool OreHaulRoutes::hasRoute(const MineFacility& mineFacility) const
{
	const auto& routeTable = getRouteTable();
	return routeTable.find(&mineFacility) != routeTable.end();
}


const Route& OreHaulRoutes::getRoute(const MineFacility& mineFacility) const
{
	return getRouteTable().at(&mineFacility);
}


float OreHaulRoutes::getRouteCost(const MineFacility& mineFacility) const
{
	const auto& routeTable = getRouteTable();
	if (routeTable.find(&mineFacility) == routeTable.end())
	{
		return FLT_MAX;
	}
	const auto& route = routeTable.at(&mineFacility);
	return std::clamp(route.cost, 1.0f, FLT_MAX);
}


int OreHaulRoutes::getOreHaulCapacity(const MineFacility& mineFacility) const
{
	const float routeCost = getRouteCost(mineFacility);
	return static_cast<int>(constants::ShortestPathTraversalCount / routeCost) * mineFacility.assignedTrucks();
}


void OreHaulRoutes::removeMine(const MineFacility& mineFacility)
{
	getRouteTable().erase(&mineFacility);
}


void OreHaulRoutes::clear()
{
	getRouteTable().clear();
}


void OreHaulRoutes::updateRoutes()
{
	auto& routeTable = getRouteTable();
	routeTable.clear();

	const auto structureIsSmelter = [](const Structure& structure) { return structure.isSmelter(); };
	const auto structureIsOperableMine = [](const Structure& structure) { return structure.isMineFacility() && structure.isOperable(); };
	const auto& smelters = mStructureManager.getStructures(structureIsSmelter);
	const auto& mines = mStructureManager.getStructures(structureIsOperableMine);
	for (const auto* mineFacility : mines)
	{
		auto newRoute = mRouteFinder.findLowestCostRoute(mineFacility, smelters);
		if (!newRoute.isEmpty()) {
			routeTable[dynamic_cast<const MineFacility*>(mineFacility)] = newRoute;
		}
	}
}


void OreHaulRoutes::transportOreFromMines()
{
	const auto& routeTable = getRouteTable();
	for (const auto* mineFacilityPtr : mStructureManager.getStructures<MineFacility>())
	{
		auto routeIt = routeTable.find(mineFacilityPtr);
		if (routeIt != routeTable.end())
		{
			const auto& route = routeIt->second;
			auto& smelter = *route.path.back()->structure();
			auto& mineFacility = dynamic_cast<MineFacility&>(*route.path.front()->structure());

			if (!smelter.operational()) { break; }

			/* clamp route cost to minimum of 1.0f for next computation to avoid
			   unintended multiplication. */
			const float routeCost = std::clamp(routeIt->second.cost, 1.0f, FLT_MAX);

			/* intentional truncation of fractional component*/
			const int totalOreMovement = static_cast<int>(constants::ShortestPathTraversalCount / routeCost) * mineFacility.assignedTrucks();
			const int oreMovementPart = totalOreMovement / 4;
			const int oreMovementRemainder = totalOreMovement % 4;
			const auto movementCap = StorableResources{oreMovementPart, oreMovementPart, oreMovementPart, oreMovementPart + oreMovementRemainder};

			auto& mineStorage = mineFacility.storage();
			auto& smelterStored = smelter.production();

			const auto oreAvailable = smelterStored + mineStorage.cap(movementCap);
			const auto newSmelterStored = oreAvailable.cap(smelter.rawOreStorageCapacity());
			const auto movedOre = newSmelterStored - smelterStored;

			mineStorage -= movedOre;
			smelterStored = newSmelterStored;
		}
	}
}


std::vector<Tile*> OreHaulRoutes::getRouteOverlay() const
{
	std::vector<Tile*> routeTiles;
	for (auto& [_, route] : getRouteTable())
	{
		for (auto* tile : route.path)
		{
			routeTiles.push_back(tile);
		}
	}
	return routeTiles;
}
