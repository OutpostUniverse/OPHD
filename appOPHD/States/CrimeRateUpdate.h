#pragma once

#include <libOPHD/EnumDifficulty.h>

#include <vector>
#include <string>
#include <utility>


class Structure;
class Tile;


class CrimeRateUpdate
{
public:
	CrimeRateUpdate(const Difficulty& difficulty);

	void update(const std::vector<std::vector<Tile*>>& policeOverlays);

	int meanCrimeRate() const { return mMeanCrimeRate; }
	std::vector<std::pair<std::string, int>> moraleChanges() const { return mMoraleChanges; }
	std::vector<Structure*> structuresCommittingCrimes() const { return mStructuresCommittingCrimes; }

private:
	const Difficulty& mDifficulty;
	int mMeanCrimeRate{0};
	std::vector<std::pair<std::string, int>> mMoraleChanges;
	std::vector<Structure*> mStructuresCommittingCrimes;

	bool isProtectedByPolice(const std::vector<std::vector<Tile*>>& policeOverlays, Structure* structure);
	int calculateMoraleChange() const;
	void updateMoraleChanges();
};
