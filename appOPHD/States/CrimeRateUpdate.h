#pragma once

#include <libOPHD/EnumDifficulty.h>
#include <libOPHD/Population/MoraleChangeEntry.h>

#include <vector>


class Structure;
class Tile;


class CrimeRateUpdate
{
public:
	CrimeRateUpdate(const Difficulty& difficulty);

	void update(const std::vector<std::vector<Tile*>>& policeOverlays);

	int meanCrimeRate() const;
	std::vector<MoraleChangeEntry> moraleChanges() const;
	std::vector<Structure*> structuresCommittingCrimes() const;

private:
	const Difficulty& mDifficulty;
	int mMeanCrimeRate{0};
	std::vector<MoraleChangeEntry> mMoraleChanges;
	std::vector<Structure*> mStructuresCommittingCrimes;

	int calculateMoraleChange() const;
	void updateMoraleChanges();
};
