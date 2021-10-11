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
	mPopulation.clear();
}


/**
 * Populates a given population with a specified number of colonists given some base values.
 *
 * \param	role		Segment of the population to populate.
 * \param	count		Base age in months of the population to populated.
 */
void Population::addPopulation(PopulationTable::Role role, int count)
{
	mPopulation[role] += count;
}


/**
 * Population check for new children.
 * 
 * \todo	Account for nurseries when implemented.
 */
void Population::spawnChildren(int morale, int residences, int nurseries)
{
	if (residences <= 0 && nurseries <= 0) { return; }
	if (mPopulation[PopulationTable::Role::Scientist] + mPopulation[PopulationTable::Role::Worker] <= 0) { return; }

	mPopulationGrowth[PopulationTable::Role::Child] += mPopulation[PopulationTable::Role::Scientist] / 4 + mPopulation[PopulationTable::Role::Worker] / 2;

	int divisor = moraleModifierTable[moraleIndex(morale)].fertilityRate;

	int newChildren = mPopulationGrowth[PopulationTable::Role::Child] / divisor;
	mPopulationGrowth[PopulationTable::Role::Child] = mPopulationGrowth[PopulationTable::Role::Child] % divisor;

	mPopulation[PopulationTable::Role::Child] += newChildren;
	mBirthCount = newChildren;
}


void Population::spawnStudents()
{
	if (mPopulation[PopulationTable::Role::Child] <= 0) { return; }

	mPopulationGrowth[PopulationTable::Role::Student] += mPopulation[PopulationTable::Role::Child];

	int divisor = std::max(mPopulation.adults(), studentToAdultBase);
	divisor = ((divisor / 40) * 3 + 16) * 4;

	int newStudents = mPopulationGrowth[PopulationTable::Role::Student] / divisor;
	mPopulationGrowth[PopulationTable::Role::Student] = mPopulationGrowth[PopulationTable::Role::Student] % divisor;

	mPopulation[PopulationTable::Role::Student] += newStudents;
	mPopulation[PopulationTable::Role::Child] -= newStudents;
}


void Population::spawnAdults(int universities)
{
	if (mPopulation[PopulationTable::Role::Student] <= 0) { return; }

	mPopulationGrowth[PopulationTable::Role::Worker] += mPopulation[PopulationTable::Role::Student];

	int divisor = std::max(mPopulation.adults(), studentToAdultBase);
	divisor = ((divisor / 40) * 3 + 45) * 4;

	int newAdult = mPopulationGrowth[PopulationTable::Role::Worker] / divisor;
	mPopulationGrowth[PopulationTable::Role::Worker] = mPopulationGrowth[PopulationTable::Role::Worker] % divisor;

	// account for universities
	if (universities > 0 && randomNumber.generate(0, 100) <= studentToScientistRate)
	{
		mPopulation[PopulationTable::Role::Scientist] += newAdult;
	}
	else
	{
		mPopulation[PopulationTable::Role::Worker] += newAdult;
	}

	mPopulation[PopulationTable::Role::Student] -= newAdult;
}


void Population::spawnRetiree()
{
	int total_adults = mPopulation[PopulationTable::Role::Worker] + mPopulation[PopulationTable::Role::Scientist];

	if (total_adults <= 0) { return; }

	mPopulationGrowth[PopulationTable::Role::Retired] += total_adults / 10;

	int divisor = std::max(total_adults, adultToRetireeBase);

	divisor = ((divisor / 40) * 3 + 40) * 4;

	int retiree = mPopulationGrowth[PopulationTable::Role::Retired] / divisor;
	mPopulationGrowth[PopulationTable::Role::Retired] = mPopulationGrowth[PopulationTable::Role::Retired] % divisor;

	mPopulation[PopulationTable::Role::Retired] += retiree;

	/** Workers retire earlier than scientists. */
	const auto retirePopulationType = randomNumber.generate(0, 100) <= 45 ? PopulationTable::Role::Scientist : PopulationTable::Role::Worker;
	if (mPopulation[retirePopulationType] > 0) { mPopulation[retirePopulationType] -= retiree; }
}


void Population::killChildren(int morale, int nurseries)
{
	if (mPopulation[PopulationTable::Role::Child] <= 0) { return; }

	mPopulationDeath[PopulationTable::Role::Child] += mPopulation[PopulationTable::Role::Child];

	int divisor = moraleModifierTable[moraleIndex(morale)].mortalityRate + (nurseries * 10);

	int deaths = mPopulationDeath[PopulationTable::Role::Child] / divisor;
	mPopulationDeath[PopulationTable::Role::Child] = mPopulationDeath[PopulationTable::Role::Child] % divisor;

	mPopulation[PopulationTable::Role::Child] -= deaths;
	mDeathCount += deaths;

	if (mPopulation[PopulationTable::Role::Child] <= 0)
	{
		mPopulationDeath[PopulationTable::Role::Child] = 0;
		mPopulationGrowth[PopulationTable::Role::Student] = 0;
	}
}


void Population::killStudents(int morale, int hospitals)
{
	if (mPopulation[PopulationTable::Role::Student] <= 0) { return; }

	mPopulationDeath[PopulationTable::Role::Student] += mPopulation[PopulationTable::Role::Student];

	int divisor = moraleModifierTable[moraleIndex(morale)].mortalityRate + (hospitals * 65);

	int deaths = mPopulationDeath[PopulationTable::Role::Student] / divisor;
	mPopulationDeath[PopulationTable::Role::Student] = mPopulationDeath[PopulationTable::Role::Student] % divisor;

	mPopulation[PopulationTable::Role::Student] -= deaths;
	mDeathCount += deaths;

	if (mPopulation[PopulationTable::Role::Student] <= 0)
	{
		mPopulationDeath[PopulationTable::Role::Student] = 0;
		mPopulationGrowth[PopulationTable::Role::Worker] = 0;
	}
}


void Population::killAdults(PopulationTable::Role role, int morale, int hospitals)
{
	if (mPopulation[role] <= 0) { return; }

	mPopulationDeath[role] += mPopulation[role];
	int divisor = moraleModifierTable[moraleIndex(morale)].mortalityRate + 250 + (hospitals * 60);

	int deaths = mPopulationDeath[role] / divisor;
	mPopulationDeath[role] = mPopulationDeath[role] % divisor;

	mPopulation[role] -= deaths;
	mDeathCount += deaths;

	if (mPopulation[role] == 0)
	{
		mPopulationDeath[role] = 0;
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
	// If there's no food kill everybody (humans can survive up to 21 days without food, one turn == minimum 28 days)
	if (food == 0)
	{
		mDeathCount = mPopulation.size();
		clear();
		return 0;
	}

	int PopulationFed = food * 10;
	if (PopulationFed > mPopulation.size())
	{
		return mPopulation.size() / 10;
	}


	int populationToKill = static_cast<int>((mPopulation.size() - PopulationFed) * mStarveRate);
	if (mPopulation.size() == 1) { populationToKill = 1; }

	for (int i = 0; i < populationToKill; /**/)
	{
		std::size_t roleIndex = i % 5;

		std::size_t counter = 0;
		for (;;)
		{
			roleIndex = roleIndex + counter;
			if (roleIndex > 4) { roleIndex = 0; }

			if (mPopulation[roleIndex] > 0)
			{
				break;
			}

			++counter;
			if (counter > 4) { counter = 0; }
		}

		--mPopulation[roleIndex];
		++i;
	}

	mDeathCount = populationToKill;

	// actual amount of food used for the fed part of the population.
	return PopulationFed / 10;
}


/**
 * \return	Actual amount of food consumed.
 */
int Population::update(int morale, int food, int residences, int universities, int nurseries, int hospitals)
{
	mBirthCount = 0;
	mDeathCount = 0;

	spawnChildren(morale, residences, nurseries);
	spawnStudents();
	spawnAdults(universities);
	spawnRetiree();

	killChildren(morale, nurseries);
	killStudents(morale, hospitals);

	// Workers will die more often than scientists.
	auto employableRoleToKill = randomNumber.generate(0, 100) <= 45 ?
		PopulationTable::Role::Scientist : PopulationTable::Role::Worker;
	killAdults(employableRoleToKill, morale, hospitals);

	killAdults(PopulationTable::Role::Retired, morale, hospitals);

	return consumeFood(food);
}
