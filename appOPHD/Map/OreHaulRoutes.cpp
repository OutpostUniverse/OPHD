#include "OreHaulRoutes.h"

#include "Route.h"
#include "Tile.h"
#include "../StructureManager.h"
#include "../MapObjects/Structures/MineFacility.h"

#include <cfloat>
#include <map>
#include <algorithm>


namespace
{
	/**<
	 * The number of times a truck can traverse the shortest possible path
	 * between a mine and a smelter (adjacent to one another). A truck can move 1
	 * unit of ore per trip. The shortest path cost is 0.50f. This number
	 * represents 100 round trips between the mine/smelter for effectively 100
	 * units of ore transported per turn.
	 */
	inline constexpr float ShortestPathTraversalCount{400.0f};

	std::map<const MineFacility*, Route> routeTable;
}


OreHaulRoutes::OreHaulRoutes(TileMap& tileMap, const StructureManager& structureManager) :
	mRouteFinder{tileMap},
	mStructureManager{structureManager}
{
}


bool OreHaulRoutes::hasRoute(const MineFacility& mineFacility) const
{
	return routeTable.find(&mineFacility) != routeTable.end();
}


const Route& OreHaulRoutes::getRoute(const MineFacility& mineFacility) const
{
	return routeTable.at(&mineFacility);
}


float OreHaulRoutes::getRouteCost(const MineFacility& mineFacility) const
{
	if (routeTable.find(&mineFacility) == routeTable.end())
	{
		return FLT_MAX;
	}
	const auto& route = routeTable.at(&mineFacility);
	return route.cost;
}


int OreHaulRoutes::getOreHaulCapacity(const MineFacility& mineFacility) const
{
	const auto routeCost = getRouteCost(mineFacility);
	return static_cast<int>(ShortestPathTraversalCount / routeCost) * mineFacility.assignedTrucks();
}


void OreHaulRoutes::removeMine(const MineFacility& mineFacility)
{
	routeTable.erase(&mineFacility);
}


void OreHaulRoutes::clear()
{
	routeTable.clear();
}


void OreHaulRoutes::updateRoutes()
{
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
	for (const auto* mineFacilityPtr : mStructureManager.getStructures<MineFacility>())
	{
		auto routeIt = routeTable.find(mineFacilityPtr);
		if (routeIt != routeTable.end())
		{
			const auto& route = routeIt->second;
			auto& smelter = *route.path.back()->structure();
			auto& mineFacility = dynamic_cast<MineFacility&>(*route.path.front()->structure());

			if (!smelter.operational()) { break; }

			const int totalOreMovement = getOreHaulCapacity(mineFacility);
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
	for (auto& [_, route] : routeTable)
	{
		for (auto* tile : route.path)
		{
			routeTiles.push_back(tile);
		}
	}
	return routeTiles;
}
