#pragma once

#include "../Map/Tile.h"
#include <vector>

class PopulationPanel;
class Structure;

class CrimeRateUpdate
{
public:
	CrimeRateUpdate(PopulationPanel& populationPanel);

	void update(const std::vector<TileList>& policeOverlays);

	int moraleChange() const { return mMoraleChange; }
	std::vector<Structure*> structuresCommittingCrimes() const { return mStructuresCommittingCrimes; }

private:
	PopulationPanel& mPopulationPanel;
	int mMoraleChange{ 0 };
	std::vector<Structure*> mStructuresCommittingCrimes;

	bool isProtectedByPolice(const std::vector<TileList>& policeOverlays, Structure* structure);
	int calculateMoraleChange(int meanCrimeRate);
	void updateCrimeOnPopulationPanel(int moraleChange, int meanCrimeRate);
};
