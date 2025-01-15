#pragma once

#include "../MapObjects/Structures/FoodProduction.h"

#include <NAS2D/Signal/Signal.h>

#include <vector>
#include <array>
#include <map>
#include <string>
#include <utility>


enum class Difficulty;


class CrimeExecution
{
public:
	using Signal = NAS2D::Signal<std::string, std::string, const Structure&>;

	CrimeExecution(const Difficulty& difficulty);
	CrimeExecution(const Difficulty& difficulty, Signal::DelegateType onCrimeEvent);

	void executeCrimes(const std::vector<Structure*>& structuresCommittingCrime);
	std::vector<std::pair<std::string, int>> moraleChanges() const { return mMoraleChanges; }
	Signal::Source& crimeEventSignal() { return mCrimeEventSignal; }

protected:
	void stealFood(FoodProduction& structure);
	void stealRefinedResources(Structure& structure);
	void stealRawResources(Structure& structure);
	void stealResources(Structure& structure, const std::array<std::string, 4>& resourceNames);
	void vandalize(Structure& structure);

private:
	const Difficulty& mDifficulty;
	std::vector<std::pair<std::string, int>> mMoraleChanges;
	Signal mCrimeEventSignal;
};
