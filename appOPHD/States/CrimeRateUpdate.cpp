#include "CrimeRateUpdate.h"

#include "../Map/Tile.h"
#include "../MapObjects/Structure.h"
#include "../StructureManager.h"

#include <libOPHD/EnumDifficulty.h>
#include <libOPHD/RandomNumberGenerator.h>
#include <libOPHD/Population/MoraleChangeEntry.h>

#include <NAS2D/Utility.h>

#include <map>


namespace
{
	std::map<Difficulty, int> chanceCrimeOccursPercent
	{
		{Difficulty::Beginner, 50},
		{Difficulty::Easy, 75},
		{Difficulty::Medium, 100},
		{Difficulty::Hard, 200}
	};


	bool isProtectedByPolice(const std::vector<std::vector<Tile*>>& policeOverlays, const Structure* structure)
	{
		const auto& structureTile = structure->tile();

		for (const auto& tile : policeOverlays[static_cast<std::size_t>(structureTile.depth())])
		{
			if (tile->xy() == structureTile.xy())
			{
				return true;
			}
		}

		return false;
	}
}


CrimeRateUpdate::CrimeRateUpdate(const Difficulty& difficulty) :
	mDifficulty{difficulty}
{
}


void CrimeRateUpdate::update(const std::vector<std::vector<Tile*>>& policeOverlays)
{
	mMeanCrimeRate = 0;
	mStructuresCommittingCrimes.clear();
	mMoraleChanges.clear();

	const auto& structuresWithCrime = NAS2D::Utility<StructureManager>::get().structuresWithCrime();

	// Colony will not have a crime rate until at least one structure that supports crime is built
	if (structuresWithCrime.empty())
	{
		return;
	}

	double accumulatedCrime{0};

	for (auto* structure : structuresWithCrime)
	{
		int crimeRateChange = isProtectedByPolice(policeOverlays, structure) ? -1 : 1;
		structure->increaseCrimeRate(crimeRateChange);

		// Crime Rate of 0% means no crime
		// Crime Rate of 100% means crime occurs 10% of the time on medium difficulty
		// chanceCrimeOccurs multiplier increases or decreases chance based on difficulty
		if (structure->crimeRate() * chanceCrimeOccursPercent[mDifficulty] + randomNumber.generate<int>(0, 100000) > 100000)
		{
			mStructuresCommittingCrimes.push_back(structure);
		}

		accumulatedCrime += structure->crimeRate();
	}

	mMeanCrimeRate = static_cast<int>(accumulatedCrime / static_cast<double>(structuresWithCrime.size()));

	updateMoraleChanges();
}


int CrimeRateUpdate::meanCrimeRate() const
{
	return mMeanCrimeRate;
}


std::vector<MoraleChangeEntry> CrimeRateUpdate::moraleChanges() const
{
	return mMoraleChanges;
}


std::vector<Structure*> CrimeRateUpdate::structuresCommittingCrimes() const
{
	return mStructuresCommittingCrimes;
}


int CrimeRateUpdate::calculateMoraleChange() const
{
	if (mMeanCrimeRate > 50)
	{
		// Reduce morale by 1 for every 10% above 50%
		return -1 * (mMeanCrimeRate / 10 - 4);
	}
	else if (mMeanCrimeRate < 10)
	{
		return 1;
	}

	return 0;
}


void CrimeRateUpdate::updateMoraleChanges()
{
	auto moraleChange = calculateMoraleChange();

	if (moraleChange > 0)
	{
		mMoraleChanges.push_back({"Low Crime Rate", moraleChange});
	}
	else if (moraleChange < 0)
	{
		mMoraleChanges.push_back({"High Crime Rate", moraleChange});
	}
}
