#pragma once

#include "../Map/Tile.h"
#include <vector>

class PopulationPanel;
class Structure;

class CrimeRateUpdate
{
public:
	void update(const std::vector<TileList>& policeOverlays, PopulationPanel& populationPanel);

	int getMoraleChange() const { return mMoraleChange; }

private:
	int mMoraleChange = 0;

	bool isProtectedByPolice(const std::vector<TileList>& policeOverlays, Structure* structure);
	int calculateMoraleChange(int meanCrimeRate);
	void setPopulationPanel(int moraleChange, int meanCrimeRate, PopulationPanel& populationPanel);
};
