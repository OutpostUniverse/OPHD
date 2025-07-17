#include "RouteFinder.h"

#include "Route.h"
#include "Tile.h"
#include "TileMap.h"
#include "../MapObjects/Structures/OreRefining.h"
#include "../MicroPather/micropather.h"

#include <libOPHD/EnumTerrainType.h>
#include <libOPHD/DirectionOffset.h>

#include <cmath>


namespace
{
	std::vector<Route> findRoutes(micropather::MicroPather* solver, const Structure* mineFacility, const std::vector<OreRefining*>& smelters)
	{
		auto& start = mineFacility->tile();

		std::vector<Route> routeList;

		for (const auto* smelter : smelters)
		{
			if (!smelter->operational()) { continue; }

			auto& end = smelter->tile();

			Route route;
			solver->Reset();
			solver->Solve(&start, &end, reinterpret_cast<std::vector<void*>*>(&route.path), &route.cost);

			if (!route.isEmpty()) { routeList.push_back(route); }
		}

		return routeList;
	}


	Route findLowestCostRoute(const std::vector<Route>& routeList)
	{
		if (routeList.empty()) { return Route(); }

		return std::ranges::min(routeList, {}, [](const Route& a) { return a.cost; });
	}
}


class TileMapGraph : public micropather::Graph
{
public:
	TileMapGraph(TileMap& tileMap) :
		mTileMap{tileMap}
	{
	}


	/**
	* Implements MicroPather interface.
	*
	* \warning	Assumes stateStart and stateEnd are never nullptr.
	*/
	float LeastCostEstimate(void* stateStart, void* stateEnd) override
	{
		return sqrtf(static_cast<float>((static_cast<Tile*>(stateEnd)->xy() - static_cast<Tile*>(stateStart)->xy()).lengthSquared()));
	}


	void AdjacentCost(void* state, std::vector<micropather::StateCost>* adjacent) override
	{
		auto& tile = *static_cast<Tile*>(state);
		const auto tilePosition = tile.xy();

		for (const auto& offset : DirectionClockwise4)
		{
			const auto position = tilePosition + offset;
			if (!NAS2D::Rectangle{{0, 0}, mTileMap.size()}.contains(position))
			{
				continue;
			}

			auto& adjacentTile = mTileMap.getTile({position, 0});
			float cost = adjacentTile.movementCost();

			micropather::StateCost nodeCost = {&adjacentTile, cost};
			adjacent->push_back(nodeCost);
		}
	}


	void PrintStateInfo(void* /*state*/) override
	{
	}

private:
	TileMap& mTileMap;
};


Route findLowestCostRoute(micropather::MicroPather* solver, const Structure* mineFacility, const std::vector<OreRefining*>& smelters)
{
	auto routeList = findRoutes(solver, mineFacility, smelters);
	auto newRoute = findLowestCostRoute(routeList);
	return newRoute;
}


bool routeObstructed(Route& route)
{
	for (auto tilePtr : route.path)
	{
		auto& tile = *tilePtr;

		// \note	Tile being occupied by a robot is not an obstruction for the
		//			purposes of routing/pathing.
		if (tile.hasStructure() && !tile.structure()->isRoad()) { return true; }
		if (tile.index() == TerrainType::Impassable) { return true; }
	}

	return false;
}


RouteFinder::RouteFinder(TileMap& tileMap) :
	mTileMapGraph{std::make_unique<TileMapGraph>(tileMap)},
	mPathSolver{std::make_unique<micropather::MicroPather>(mTileMapGraph.get(), 250, 6, false)}
{
}


RouteFinder::~RouteFinder()
{
}


Route RouteFinder::findLowestCostRoute(const Structure* mineFacility, const std::vector<OreRefining*>& smelters)
{
	return ::findLowestCostRoute(mPathSolver.get(), mineFacility, smelters);
}
