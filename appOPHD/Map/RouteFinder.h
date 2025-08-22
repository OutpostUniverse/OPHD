#pragma once

#include <vector>
#include <memory>


namespace micropather
{
	class MicroPather;
}


struct Route;
class Structure;
class TileMap;
class TileMapGraph;


Route findLowestCostRoute(micropather::MicroPather* solver, const Structure* mineFacility, const std::vector<Structure*>& smelters);
bool routeObstructed(Route& route);


class RouteFinder
{
public:
	RouteFinder(TileMap& tileMap);
	~RouteFinder();

	Route findLowestCostRoute(const Structure* mineFacility, const std::vector<Structure*>& smelters);

private:
	std::unique_ptr<TileMapGraph> mTileMapGraph;
	std::unique_ptr<micropather::MicroPather> mPathSolver;
};
