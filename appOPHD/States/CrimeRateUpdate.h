#pragma once

#include "../Common.h"

#include <vector>
#include <map>
#include <string>
#include <utility>


class Structure;
class Tile;


class CrimeRateUpdate
{
public:
	void update(const std::vector<std::vector<Tile*>>& policeOverlays);

	int meanCrimeRate() const { return mMeanCrimeRate; }
	std::vector<std::pair<std::string, int>> moraleChanges() const { return mMoraleChanges; }
	void difficulty(Difficulty difficulty) { mDifficulty = difficulty; }
	std::vector<Structure*> structuresCommittingCrimes() const { return mStructuresCommittingCrimes; }

private:
	// Lower number indicates criminal activity occurs more often
	std::map<Difficulty, float> chanceCrimeOccurs
	{
		{Difficulty::Beginner, 0.5f},
		{Difficulty::Easy, 0.75f},
		{Difficulty::Medium, 1.0f},
		{Difficulty::Hard, 2.0f}
	};

	Difficulty mDifficulty{Difficulty::Medium};
	int mMeanCrimeRate{0};
	std::vector<std::pair<std::string, int>> mMoraleChanges;
	std::vector<Structure*> mStructuresCommittingCrimes;

	bool isProtectedByPolice(const std::vector<std::vector<Tile*>>& policeOverlays, Structure* structure);
	int calculateMoraleChange();
	void updateMoraleChanges();
};
