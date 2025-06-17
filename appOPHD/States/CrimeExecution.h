#pragma once

#include <NAS2D/Signal/Delegate.h>

#include <vector>
#include <array>
#include <string>
#include <utility>


enum class Difficulty;
class Structure;
class FoodProduction;


class CrimeExecution
{
public:
	using CrimeEventDelegate = NAS2D::Delegate<void(std::string, std::string, const Structure&)>;

	CrimeExecution(const Difficulty& difficulty, CrimeEventDelegate crimeEventHandler);

	void executeCrimes(const std::vector<Structure*>& structuresCommittingCrime);
	std::vector<std::pair<std::string, int>> moraleChanges() const { return mMoraleChanges; }

protected:
	void stealFood(FoodProduction& structure);
	void stealRefinedResources(Structure& structure);
	void stealRawResources(Structure& structure);
	void stealResources(Structure& structure, const std::array<std::string, 4>& resourceNames);
	void vandalize(Structure& structure);

private:
	const Difficulty& mDifficulty;
	std::vector<std::pair<std::string, int>> mMoraleChanges;
	CrimeEventDelegate mCrimeEventHandler;
};
