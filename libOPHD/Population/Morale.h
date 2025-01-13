#pragma once

#include <cstdint>
#include <string>

/**
 * Morale modifier values.
 */
struct MoraleModifier
{
	int researchBonus{0};
	int productionBonus{0};
	int fertilityRate{0};
	int mortalityRate{0};
};

struct MoraleChangeEntry
{
	std::string description{};
	int value{0};
};

class Morale
{
public:
	Morale();
	Morale(int currentMorale, int previousMorale);
	int currentMorale() const;
	int previousMorale() const;
	void adjustMorale(int diff);

	// Should be called after all morale calculations are done for the turn to apply the morale changes but before its needed to update the UI
	void commitMoraleChanges();


private:
	int mCurrentMorale;
	int mPreviousMorale;
	int mMoraleAccumulator;
};
