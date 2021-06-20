#pragma once

#include "../Map/Tile.h"
#include "../Common.h"
#include <vector>
#include <map>

class PopulationPanel;
class Structure;


class CrimeRateUpdate
{
public:
	CrimeRateUpdate(PopulationPanel& populationPanel);

	void update(const std::vector<TileList>& policeOverlays);

	int moraleChange() const { return mMoraleChange; }
	void difficulty(Difficulty difficulty) { mDifficulty = difficulty; }
	std::vector<Structure*> structuresCommittingCrimes() const { return mStructuresCommittingCrimes; }

private:
	// Lower number indicates criminal activity occurs more often
	std::map<Difficulty, float> chanceCrimeOccurs
	{
		{ Difficulty::Beginner, 0.5f },
		{ Difficulty::Easy, 0.75f },
		{ Difficulty::Medium, 1.0f },
		{ Difficulty::Hard, 2.0f }
	};

	Difficulty mDifficulty{ Difficulty::Medium };
	PopulationPanel& mPopulationPanel;
	int mMoraleChange{ 0 };
	std::vector<Structure*> mStructuresCommittingCrimes;

	bool isProtectedByPolice(const std::vector<TileList>& policeOverlays, Structure* structure);
	int calculateMoraleChange(int meanCrimeRate);
	void updateCrimeOnPopulationPanel(int moraleChange, int meanCrimeRate);
};
