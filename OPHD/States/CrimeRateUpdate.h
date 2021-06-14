#pragma once

#include "../Map/Tile.h"
#include <vector>

class PopulationPanel;
class Structure;

class CrimeRateUpdate
{
public:
	// Returns change in current morale caused by colony's mean crime rate
	int update(const std::vector<TileList>& policeOverlays, PopulationPanel& populationPanel);

private:
	bool isProtectedByPolice(const std::vector<TileList>& policeOverlays, Structure* structure);
	int calculateMoraleChange(int meanCrimeRate);
	void setPopulationPanel(int moraleChange, int meanCrimeRate, PopulationPanel& populationPanel);
};
