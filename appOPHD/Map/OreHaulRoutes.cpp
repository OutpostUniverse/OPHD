#include "OreHaulRoutes.h"

#include "Route.h"
#include "Tile.h"
#include "../StructureManager.h"
#include "../MapObjects/Structures/MineFacility.h"

#include <map>
#include <algorithm>
#include <stdexcept>


namespace
{
	/**<
	 * The number of times a truck can traverse the shortest possible path
	 * between a mine and a smelter (adjacent to one another). A truck can move 1
	 * unit of ore per trip. The shortest path cost is 0.50f. This number
	 * represents 100 round trips between the mine/smelter for effectively 100
	 * units of ore transported per turn.
	 */
	inline constexpr int ShortestPathTraversalCount{100};

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
	if (!hasRoute(mineFacility))
	{
		throw std::runtime_error("Called getRoute when no valid route");
	}
	return routeTable.at(&mineFacility);
}


int OreHaulRoutes::getRouteCost(const MineFacility& mineFacility) const
{
	return getRoute(mineFacility).cost;
}


int OreHaulRoutes::getOreHaulCapacity(const MineFacility& mineFacility) const
{
	if (!hasRoute(mineFacility)) { return 0; }
	const auto routeCost = getRouteCost(mineFacility);
	return ShortestPathTraversalCount * mineFacility.assignedTrucks() / routeCost;
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

			const int oreHaulCapacity = getOreHaulCapacity(mineFacility);
			const auto movementCap = StorableResources{oreHaulCapacity, oreHaulCapacity, oreHaulCapacity, oreHaulCapacity};

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
