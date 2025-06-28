#pragma once

#include <NAS2D/Signal/Delegate.h>

#include <vector>
#include <array>
#include <string>


enum class Difficulty;
struct MoraleChangeEntry;
class Structure;
class FoodProduction;


class CrimeExecution
{
public:
	using CrimeEventDelegate = NAS2D::Delegate<void(std::string, std::string, const Structure&)>;

	CrimeExecution(const Difficulty& difficulty, CrimeEventDelegate crimeEventHandler);

	std::vector<MoraleChangeEntry> moraleChanges() const;
	void executeCrimes(const std::vector<Structure*>& structuresCommittingCrime);

protected:
	void stealFood(FoodProduction& structure);
	void stealRefinedResources(Structure& structure);
	void stealRawResources(Structure& structure);
	void stealResources(Structure& structure, const std::array<std::string, 4>& resourceNames);
	void vandalize(Structure& structure);

private:
	const Difficulty& mDifficulty;
	std::vector<MoraleChangeEntry> mMoraleChanges;
	CrimeEventDelegate mCrimeEventHandler;
};
