#include "RouteFinder.h"

#include "Route.h"
#include "Tile.h"
#include "TileMap.h"
#include "../MapObjects/Structure.h"
#include "../MicroPather/micropather.h"

#include <libOPHD/EnumIntegrityLevel.h>
#include <libOPHD/EnumTerrainType.h>
#include <libOPHD/DirectionOffset.h>

#include <NAS2D/Math/Rectangle.h>

#include <cmath>


namespace
{
	constexpr float RouteBaseCost{2.0f};


	std::vector<Route> findRoutes(micropather::MicroPather* solver, const Structure* mineFacility, const std::vector<Structure*>& smelters)
	{
		auto& start = mineFacility->tile();

		std::vector<Route> routeList;

		for (const auto* smelter : smelters)
		{
			if (!smelter->operational()) { continue; }

			auto& end = smelter->tile();

			Route route;
			float floatCost;
			solver->Reset();
			solver->Solve(&start, &end, reinterpret_cast<std::vector<void*>*>(&route.path), &floatCost);

			if (!route.isEmpty())
			{
				route.cost = static_cast<int>(floatCost);
				routeList.push_back(route);
			}
		}

		return routeList;
	}


	Route findLowestCostRoute(const std::vector<Route>& routeList)
	{
		if (routeList.empty()) { return Route(); }

		return std::ranges::min(routeList, {}, [](const Route& a) { return a.cost; });
	}


	Route findLowestCostRoute(micropather::MicroPather* solver, const Structure* mineFacility, const std::vector<Structure*>& smelters)
	{
		auto routeList = findRoutes(solver, mineFacility, smelters);
		auto newRoute = findLowestCostRoute(routeList);
		return newRoute;
	}


	float tileMovementCost(const Tile& tile)
	{
		if (tile.hasStructure() && tile.structure()->isRoad())
		{
			Structure& road = *tile.structure();

			if (road.integrityLevel() >= IntegrityLevel::Good) { return 2.0f; }
			else if (road.integrityLevel() >= IntegrityLevel::Worn) { return 3.0f; }
			else { return RouteBaseCost * static_cast<float>(TerrainType::Difficult) + 4.0f; }
		}

		if (tile.hasMapObject() && (!tile.hasStructure() || (!tile.structure()->isMineFacility() && !tile.structure()->isSmelter())))
		{
			return FLT_MAX;
		}

		if (tile.index() == TerrainType::Impassable)
		{
			return FLT_MAX;
		}

		return RouteBaseCost * static_cast<float>(tile.index()) + 4.0f;
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
			float cost = tileMovementCost(adjacentTile);

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


RouteFinder::RouteFinder(TileMap& tileMap) :
	mTileMapGraph{std::make_unique<TileMapGraph>(tileMap)},
	mPathSolver{std::make_unique<micropather::MicroPather>(mTileMapGraph.get(), 250, 6, false)}
{
}


RouteFinder::~RouteFinder()
{
}


Route RouteFinder::findLowestCostRoute(const Structure* mineFacility, const std::vector<Structure*>& smelters)
{
	return ::findLowestCostRoute(mPathSolver.get(), mineFacility, smelters);
}
