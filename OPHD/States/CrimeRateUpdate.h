#pragma once

#include "../Map/Tile.h"
#include <vector>

class PopulationPanel;

class CrimeRateUpdate
{
public:
	// Returns change in current morale caused by colony's mean crime rate
	int update(const std::vector<TileList>& policeOverlays, PopulationPanel& populationPanel);
};
