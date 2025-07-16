#pragma once

#include <vector>
#include <memory>


namespace micropather
{
	class MicroPather;
}


struct Route;
class Structure;
class OreRefining;
class TileMap;


Route findLowestCostRoute(micropather::MicroPather* solver, const Structure* mineFacility, const std::vector<OreRefining*>& smelters);
bool routeObstructed(Route& route);


class RouteFinder
{
public:
	RouteFinder(TileMap& tileMap);
	~RouteFinder();

	Route findLowestCostRoute(const Structure* mineFacility, const std::vector<OreRefining*>& smelters);

private:
	TileMap& mTileMap;
	std::unique_ptr<micropather::MicroPather> mPathSolver;
};
