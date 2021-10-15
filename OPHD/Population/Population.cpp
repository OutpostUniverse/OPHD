#include "Population.h"

#include "../RandomNumberGenerator.h"

#include <algorithm>
#include <iostream>


namespace
{
	const int studentToScientistRate = 35;
	const int studentToAdultBase = 190;
	const int adultToRetireeBase = 2000;

	const std::array moraleModifierTable{
		MoraleModifier{50, 50, 110, 80},  // Excellent
		MoraleModifier{25, 25, 90, 75},   // Good
		MoraleModifier{0, 0, 60, 40},     // Fair
		MoraleModifier{-25, -25, 40, 20}, // Poor
		MoraleModifier{-50, -50, 20, 10}  // Terrible
	};


	/**
	 * Convenience function to cast a MoraleLevel enumerator
	 * into an array index.
	 */
	std::size_t moraleIndex(int morale)
	{
		return static_cast<std::size_t>(std::clamp(morale, 1, 999) / 200);
	}
}


/**
 * Clears entire population.
 */
void Population::clear()
{
	mPopulation = {};
}


const PopulationTable& Population::getPopulations() const
{
	return mPopulation;
}


void Population::addPopulation(const PopulationTable& population)
{
	mPopulation += population;
}


void Population::spawnPopulation(int morale, int residences, int nurseries, int universities)
{
	const int growthChild = (residences > 0 || nurseries > 0) ?
		mPopulation.scientist / 4 + mPopulation.worker / 2 : 0;

	// account for universities
	const auto newAdultRole = (universities > 0 && randomNumber.generate(0, 100) <= studentToScientistRate) ?
		PopulationTable::Role::Scientist : PopulationTable::Role::Worker;

	int total_adults = mPopulation.worker + mPopulation.scientist;

	int divisorChild = moraleModifierTable[moraleIndex(morale)].fertilityRate;
	int divisorStudent = ((std::max(mPopulation.adults(), studentToAdultBase) / 40) * 3 + 16) * 4;
	int divisorAdult = ((std::max(mPopulation.adults(), studentToAdultBase) / 40) * 3 + 45) * 4;
	int divisorRetiree = ((std::max(total_adults, adultToRetireeBase) / 40) * 3 + 40) * 4;

	const auto newChildren = spawnRole(PopulationTable::Role::Child, growthChild, divisorChild);
	const auto newStudents = spawnRole(PopulationTable::Role::Student, mPopulation.child, divisorStudent);
	const auto newAdult = spawnRole(newAdultRole, mPopulation.student, divisorAdult);
	const auto retiree = spawnRole(PopulationTable::Role::Retired, total_adults / 10, divisorRetiree);

	mBirthCount = newChildren;
	mPopulation.child -= newStudents;
	mPopulation.student -= newAdult;

	/** Workers retire earlier than scientists. */
	const auto retireRole = randomNumber.generate(0, 100) <= 45 ?
		PopulationTable::Role::Scientist : PopulationTable::Role::Worker;
	if (mPopulation[retireRole] > 0) { mPopulation[retireRole] -= retiree; }
}


int Population::spawnRole(PopulationTable::Role role, int growth, int divisor)
{
	mPopulationGrowth[role] += growth;
	const auto newRoleCount = mPopulationGrowth[role] / divisor;
	mPopulationGrowth[role] = mPopulationGrowth[role] % divisor;
	mPopulation[role] += newRoleCount;
	return newRoleCount;
}


void Population::killRole(PopulationTable::Role role, int divisor)
{
	mPopulationDeath[role] += mPopulation[role];

	int deaths = std::min(mPopulationDeath[role] / divisor, mPopulation[role]);
	mPopulationDeath[role] = mPopulationDeath[role] % divisor;

	mPopulation[role] -= deaths;
	mDeathCount += deaths;

	if (mPopulation[role] == 0)
	{
		mPopulationDeath[role] = 0;
	}
}


void Population::killPopulation(int morale, int nurseries, int hospitals)
{
	const auto mortalityRate = moraleModifierTable[moraleIndex(morale)].mortalityRate;

	int divisorChild = mortalityRate + (nurseries * 10);
	int divisorStudent = mortalityRate + (hospitals * 65);
	int divisorAdult = mortalityRate + 250 + (hospitals * 60);

	// Workers will die more often than scientists.
	auto employableRoleToKill = randomNumber.generate(0, 100) <= 45 ?
		PopulationTable::Role::Scientist : PopulationTable::Role::Worker;

	killRole(PopulationTable::Role::Child, divisorChild);
	killRole(PopulationTable::Role::Student, divisorStudent);
	killRole(employableRoleToKill, divisorAdult);
	killRole(PopulationTable::Role::Retired, divisorAdult);

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
int Population::consumeFood(int food)
{
	const int PopulationPerFood = 10;
	const int populationFed = food * PopulationPerFood;
	if (populationFed >= mPopulation.size())
	{
		// Round up food consumption
		return (mPopulation.size() + (PopulationPerFood - 1)) / PopulationPerFood;
	}

	// If there's no food kill everybody (humans can survive up to 21 days without food, one turn == minimum 28 days)
	if (food == 0)
	{
		mDeathCount += mPopulation.size();
		clear();
		return 0;
	}

	const int populationUnfed = mPopulation.size() - populationFed;
	const int populationToKill = static_cast<int>(populationUnfed * mStarveRate);
	if (mPopulation.size() == 1) { populationToKill = 1; }
	mDeathCount += populationToKill;

	for (int i = populationToKill; i > 0; mStarveRoleIndex = (mStarveRoleIndex + 1) % 5)
	{
		if (mPopulation[mStarveRoleIndex] > 0)
		{
			--mPopulation[mStarveRoleIndex];
			--i;
		}
	}

	// actual amount of food used for the fed part of the population.
	return food;
}


/**
 * \return	Actual amount of food consumed.
 */
int Population::update(int morale, int food, int residences, int universities, int nurseries, int hospitals)
{
	mBirthCount = 0;
	mDeathCount = 0;

	spawnPopulation(morale, residences, nurseries, universities);
	killPopulation(morale, nurseries, hospitals);

	return consumeFood(food);
}
