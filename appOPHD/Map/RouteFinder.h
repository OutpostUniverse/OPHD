#pragma once

#include "../MicroPather/micropather.h"

#include <vector>


struct Route;
class Structure;
class OreRefining;


Route findLowestCostRoute(micropather::MicroPather* solver, const Structure* mineFacility, const std::vector<OreRefining*>& smelters);
bool routeObstructed(Route& route);
