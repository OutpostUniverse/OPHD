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

	int getMoraleChange() const { return mMoraleChange; }
	std::vector<Structure*> getStructuresCommittingCrimes() const { return mStructuresCommittingCrimes; }

private:
	PopulationPanel& mPopulationPanel;
	int mMoraleChange{ 0 };
	std::vector<Structure*> mStructuresCommittingCrimes;

	bool isProtectedByPolice(const std::vector<TileList>& policeOverlays, Structure* structure);
	int calculateMoraleChange(int meanCrimeRate);
	void setPopulationPanel(int moraleChange, int meanCrimeRate);
};
