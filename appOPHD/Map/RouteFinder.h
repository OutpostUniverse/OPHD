#pragma once

#include <vector>


namespace micropather
{
	class MicroPather;
}


struct Route;
class Structure;
class OreRefining;


Route findLowestCostRoute(micropather::MicroPather* solver, const Structure* mineFacility, const std::vector<OreRefining*>& smelters);
bool routeObstructed(Route& route);
