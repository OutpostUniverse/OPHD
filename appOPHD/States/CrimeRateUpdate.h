#pragma once

#include <vector>


enum class Difficulty;
struct MoraleChangeEntry;
class Structure;


class CrimeRateUpdate
{
public:
	CrimeRateUpdate(const Difficulty& difficulty);

	void update();

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
