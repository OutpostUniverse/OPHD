#pragma once

#include "../Map/Tile.h"
#include <vector>

class PopulationPanel;

namespace CrimeRate
{
	// Returns change in current morale caused by colony's mean crime rate
	int update(const std::vector<TileList>& policeOverlays, PopulationPanel& populationPanel);
}
