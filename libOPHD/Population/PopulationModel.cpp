#include "PopulationModel.h"
#include "Morale.h"

#include "../RandomNumberGenerator.h"

#include <algorithm>
#include <array>
#include <limits>
#include <stdexcept>
#include <string>


namespace
{
	constexpr auto StudentToScientistRate = 35;
	constexpr auto StudentToAdultBase = 190;
	constexpr auto AdultToRetireeBase = 2000;
	constexpr auto MinRetireeGrowthThreshold = 1200; // Prevents retirees for approximately 150 turns

	const std::array MoraleModifierTable {
		MoraleModifier{0, 0, 30, 110},	// Excellent
		MoraleModifier{0, 0, 40, 90},	// Good
		MoraleModifier{0, 0, 50, 70},	// Fair
		MoraleModifier{0, 0, 70, 50},	// Poor
		MoraleModifier{0, 0, 90, 30}	// Terrible
	};


	/**
	 * Convenience function to cast a MoraleLevel enumerator
	 * into an array index.
	 */
	std::size_t moraleIndex(int morale)
	{
		return static_cast<std::size_t>(4 - (std::clamp(morale, 0, 999) / 200));
	}


	void assertNoOverritirement(int retirees, int employable)
	{
		if (retirees > employable)
		{
			throw std::runtime_error("Retiring more people than employable population: Retiring: " + std::to_string(retirees));
		}
	}


	void retireAdults(int toRetire, PopulationTable& population)
	{
		while (toRetire > 0)
		{
			/** Workers retire earlier than scientists. */
			auto& retireRole = (randomNumber.generate(0, 100) <= 45) ? population.scientist : population.worker;

			if (retireRole > 0)
			{
				--retireRole;
				--toRetire;
			}
		}
	}


	int childGrowth(int residences, int nurseries, PopulationTable& population)
	{
		return (residences > 0 || nurseries > 0) ? population.scientist / 4 + population.worker / 2 : 0;
	}
}


const PopulationTable& PopulationModel::getPopulations() const
{
	return mPopulation;
}


void PopulationModel::addPopulation(const PopulationTable& population)
{
	mPopulation += population;
}

void PopulationModel::removePopulation(const PopulationTable& population)
{
	mPopulation -= population.cap(mPopulation);
}


void PopulationModel::spawnPopulation(int morale, int residences, int nurseries, int universities)
{
	const auto growthChild = childGrowth(residences, nurseries, mPopulation);

	const int convertRate = (universities > 0) ? StudentToScientistRate : 0;
	const int growthWorker = mPopulation.student * (100 - convertRate) / 100;
	const int growthScientist = mPopulation.student * convertRate / 100;

	const int totalAdults = mPopulation.worker + mPopulation.scientist;

	const int divisorChild = MoraleModifierTable[moraleIndex(morale)].fertilityCost;
	const int divisorStudent = ((std::max(mPopulation.adults(), StudentToAdultBase) / 40) * 3 + 13) * 4;
	const int divisorAdult = ((std::max(mPopulation.adults(), StudentToAdultBase) / 40) * 3 + 38) * 4;
	const int divisorRetiree = (mPopulationGrowth.retiree < MinRetireeGrowthThreshold) ?
		std::numeric_limits<int>::max() : ((std::max(totalAdults, AdultToRetireeBase) / 40) * 3 + 40) * 4;

	const auto newRoles = spawnRoles(
		{ growthChild, mPopulation.child, growthWorker, growthScientist, totalAdults / 10 },
		{ divisorChild, divisorStudent, divisorAdult, divisorAdult, divisorRetiree }
	);

	mBirthCount = newRoles.child;
	mPopulation.child -= newRoles.student;
	mPopulation.student -= (newRoles.worker + newRoles.scientist);

	assertNoOverritirement(newRoles.retiree, mPopulation.employable());
	retireAdults(newRoles.retiree, mPopulation);
}


PopulationTable PopulationModel::spawnRoles(const PopulationTable& growth, const PopulationTable& divisor)
{
	mPopulationGrowth += growth;
	const auto newRoles = mPopulationGrowth / divisor;
	mPopulationGrowth = mPopulationGrowth % divisor;
	mPopulation += newRoles;
	return newRoles;
}


void PopulationModel::killRoles(const PopulationTable& divisor)
{
	mPopulationDeath += mPopulation;

	const auto deaths = (mPopulationDeath / divisor).cap(mPopulation);
	mPopulationDeath = mPopulationDeath % divisor;

	mPopulation -= deaths;
	mDeathCount += deaths.size();

	const auto roleCount = sizeof(PopulationTable) / sizeof(int);
	for (std::size_t role = 0; role < roleCount; ++role)
	{
		if (mPopulation[role] == 0)
		{
			mPopulationDeath[role] = 0;
		}
	}
}


void PopulationModel::killPopulation(int morale, int nurseries, int hospitals)
{
	const auto mortalityRate = MoraleModifierTable[moraleIndex(morale)].mortalityResistance;

	const int divisorChild = mortalityRate + (nurseries * 10);
	const int divisorStudent = mortalityRate + (hospitals * 65);
	const int divisorAdult = mortalityRate + 250 + (hospitals * 60);

	killRoles({divisorChild, divisorStudent, divisorAdult * 2 - 50, divisorAdult * 2 + 50, divisorAdult});

	if (mPopulation.child <= 0)
	{
		mPopulationGrowth.student = 0;
	}
	if (mPopulation.student <= 0)
	{
		mPopulationGrowth.worker = 0;
	}
}


/**
 * Determine how much food should be consumed and kill off any population that
 * starves.
 *
 * \return	Actual amount of food consumed.
 */
int PopulationModel::consumeFood(int food)
{
	const int PopulationPerFood = 10;
	const int populationFed = food * PopulationPerFood;
	const int populationUnfed = mPopulation.size() - populationFed;
	const int minKill = std::clamp(populationUnfed, 0, 1);
	const int populationToKill = std::clamp(static_cast<int>(static_cast<float>(populationUnfed) * mStarveRate), minKill, mPopulation.size());
	mDeathCount += populationToKill;

	for (int i = populationToKill; i > 0; mStarveRoleIndex = (mStarveRoleIndex + 1) % 5)
	{
		if (mPopulation[mStarveRoleIndex] > 0)
		{
			--mPopulation[mStarveRoleIndex];
			--i;
		}
	}

	// Round up food consumption for remaining people
	return (mPopulation.size() + (PopulationPerFood - 1)) / PopulationPerFood;
}


/**
 * \return	Actual amount of food consumed.
 */
int PopulationModel::update(const UpdateParameters& paremters)
{
	mBirthCount = 0;
	mDeathCount = 0;

	spawnPopulation(paremters.morale, paremters.residences, paremters.nurseries, paremters.universities);
	killPopulation(paremters.morale, paremters.nurseries, paremters.hospitals);

	return consumeFood(paremters.food);
}
