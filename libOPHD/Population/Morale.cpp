#include "Morale.h"
#include "../OPHD/Constants/Numbers.h"

#include <algorithm>


Morale::Morale() :
	Morale(constants::DefaultStartingMorale, constants::DefaultStartingMorale)
{
}


Morale::Morale(int currentMorale, int previousMorale) :
	mCurrentMorale{currentMorale},
	mPreviousMorale{previousMorale},
	mMoraleAccumulator{0}
{
}


int Morale::currentMorale() const
{
	return std::clamp(mCurrentMorale, 0, constants::MaximumMorale);
}


int Morale::previousMorale() const
{
	return std::clamp(mPreviousMorale, 0, constants::MaximumMorale);
}


void Morale::adjustMorale(int diff)
{
	mMoraleAccumulator += diff;
}


void Morale::journalMoraleChange(const MoraleChangeEntry& entry)
{
	mMoraleChangeJournal.push_back(entry);
}


void Morale::closeJournal()
{
	mMoraleChangeJournal.clear();
}


void Morale::commitMoraleChanges()
{
	mPreviousMorale = currentMorale();

	for (const auto& entry : mMoraleChangeJournal)
	{
		mMoraleAccumulator += entry.value;
	}

	mCurrentMorale = std::clamp(mCurrentMorale + mMoraleAccumulator, 0, constants::MaximumMorale);
	mMoraleAccumulator = 0;
}
