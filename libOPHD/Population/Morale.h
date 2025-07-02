#pragma once

#include <cstdint>
#include <vector>
#include <string>


struct MoraleChangeEntry;


struct MoraleModifier
{
	int researchBonus{0};
	int productionBonus{0};
	int fertilityRate{0};
	int mortalityRate{0};
};


class Morale
{
public:
	Morale();
	Morale(int currentMorale, int previousMorale);
	int currentMorale() const;
	int previousMorale() const;
	void journalMoraleChange(MoraleChangeEntry entry);
	const std::vector<MoraleChangeEntry>& moraleChangeJournal() const;

	// Clears the morale change journal vector of all entries
	void closeJournal();

	// Should be called after all morale calculations are done for the turn to apply the morale changes but before its needed to update the UI
	void commitMoraleChanges();


private:
	int mCurrentMorale;
	int mPreviousMorale;
	int mMoraleAccumulator;
	std::vector<MoraleChangeEntry> mMoraleChangeJournal;
};
