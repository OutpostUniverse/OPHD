#include "RouteFinder.h"

#include "Route.h"
#include "Tile.h"
#include "../StructureManager.h"

#include <libOPHD/EnumTerrainType.h>

#include <NAS2D/Utility.h>


namespace
{
	std::vector<Route> findRoutes(micropather::MicroPather* solver, const Structure* mineFacility, const std::vector<OreRefining*>& smelters)
	{
		auto& structureManager = NAS2D::Utility<StructureManager>::get();
		auto& start = structureManager.tileFromStructure(mineFacility);

		std::vector<Route> routeList;

		for (const auto* smelter : smelters)
		{
			if (!smelter->operational()) { continue; }

			auto& end = structureManager.tileFromStructure(smelter);

			Route route;
			solver->Reset();
			solver->Solve(&start, &end, &route.path, &route.cost);

			if (!route.empty()) { routeList.push_back(route); }
		}

		return routeList;
	}


	Route findLowestCostRoute(std::vector<Route>& routeList)
	{
		if (routeList.empty()) { return Route(); }

		return std::ranges::min(routeList, {}, [](const Route& a) { return a.cost; });
	}
}


Route findLowestCostRoute(micropather::MicroPather* solver, const Structure* mineFacility, const std::vector<OreRefining*>& smelters)
{
	auto routeList = findRoutes(solver, mineFacility, smelters);
	auto newRoute = findLowestCostRoute(routeList);
	return newRoute;
}


bool routeObstructed(Route& route)
{
	for (auto tileVoidPtr : route.path)
	{
		auto& tile = *static_cast<Tile*>(tileVoidPtr);

		// \note	Tile being occupied by a robot is not an obstruction for the
		//			purposes of routing/pathing.
		if (tile.thingIsStructure() && !tile.structure()->isRoad()) { return true; }
		if (tile.index() == TerrainType::Impassable) { return true; }
	}

	return false;
}
