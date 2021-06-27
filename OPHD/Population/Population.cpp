#include "Population.h"

#include "../RandomNumberGenerator.h"
#include <algorithm>
#include <iostream>


namespace {
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


Population::Population() :
	mBirthCount(0),
	mDeathCount(0),
	mStarveRate(0.5f)
{
	mPopulation.fill(0);
	mPopulationGrowth.fill(0);
	mPopulationDeath.fill(0);
}


/**
 * Clears entire population.
 */
void Population::clear()
{
	mPopulation.fill(0);
}


/**
 * Populates a given population with a specified number of colonists given some base values.
 *
 * \param	role		Segment of the population to populate.
 * \param	count		Base age in months of the population to populated.
 */
void Population::addPopulation(PersonRole role, int count)
{
	mPopulation[role] += count;
}


/**
 * Gets the size of the entire population.
 */
int Population::size() const
{
	int count = 0;
	for (auto& populationRole : mPopulation)
	{
		count += populationRole;
	}
	return count;
}


/**
 * Gets the size of a specific segment of the population.
 */
int Population::size(PersonRole personRole) const
{
	return mPopulation[personRole];
}


int Population::adults() const
{
	return mPopulation[PersonRole::ROLE_STUDENT] + mPopulation[PersonRole::ROLE_WORKER] + mPopulation[PersonRole::ROLE_SCIENTIST] + mPopulation[PersonRole::ROLE_RETIRED];
}


/**
 * Population check for new children.
 * 
 * \todo	Account for nurseries when implemented.
 */
void Population::spawnChildren(int morale, int residences, int nurseries)
{
	if (residences < 1 && nurseries < 1) { return; }
	if (mPopulation[PersonRole::ROLE_SCIENTIST] + mPopulation[PersonRole::ROLE_WORKER] < 1) { return; }

	mPopulationGrowth[PersonRole::ROLE_CHILD] += mPopulation[PersonRole::ROLE_SCIENTIST] / 4 + mPopulation[PersonRole::ROLE_WORKER] / 2;

	int divisor = moraleModifierTable[moraleIndex(morale)].fertilityRate;

	int newChildren = mPopulationGrowth[PersonRole::ROLE_CHILD] / divisor;
	mPopulationGrowth[PersonRole::ROLE_CHILD] = mPopulationGrowth[PersonRole::ROLE_CHILD] % divisor;

	mPopulation[PersonRole::ROLE_CHILD] += newChildren;
	mBirthCount = newChildren;
}


void Population::spawnStudents()
{
	if (mPopulation[PersonRole::ROLE_CHILD] < 1) { return; }

	mPopulationGrowth[PersonRole::ROLE_STUDENT] += mPopulation[PersonRole::ROLE_CHILD];

	int divisor = std::max(adults(), studentToAdultBase);
	divisor = ((divisor / 40) * 3 + 16) * 4;

	int newStudents = mPopulationGrowth[PersonRole::ROLE_STUDENT] / divisor;
	mPopulationGrowth[PersonRole::ROLE_STUDENT] = mPopulationGrowth[PersonRole::ROLE_STUDENT] % divisor;

	mPopulation[PersonRole::ROLE_STUDENT] += newStudents;
	mPopulation[PersonRole::ROLE_CHILD] -= newStudents;
}


void Population::spawnAdults(int universities)
{
	if (mPopulation[PersonRole::ROLE_STUDENT] < 1) { return; }

	mPopulationGrowth[PersonRole::ROLE_WORKER] += mPopulation[PersonRole::ROLE_STUDENT];

	int divisor = std::max(adults(), studentToAdultBase);
	divisor = ((divisor / 40) * 3 + 45) * 4;

	int newAdult = mPopulationGrowth[PersonRole::ROLE_WORKER] / divisor;
	mPopulationGrowth[PersonRole::ROLE_WORKER] = mPopulationGrowth[PersonRole::ROLE_WORKER] % divisor;

	// account for universities
	if (universities > 0 && randomNumber.generate(0, 100) <= studentToScientistRate)
	{
		mPopulation[PersonRole::ROLE_SCIENTIST] += newAdult;
	}
	else
	{
		mPopulation[PersonRole::ROLE_WORKER] += newAdult;
	}

	mPopulation[PersonRole::ROLE_STUDENT] -= newAdult;
}


void Population::spawnRetiree()
{
	int total_adults = mPopulation[PersonRole::ROLE_WORKER] + mPopulation[PersonRole::ROLE_SCIENTIST];

	if (total_adults < 1) { return; }

	mPopulationGrowth[PersonRole::ROLE_RETIRED] += total_adults / 10;

	int divisor = std::max(total_adults, adultToRetireeBase);

	divisor = ((divisor / 40) * 3 + 40) * 4;

	int retiree = mPopulationGrowth[PersonRole::ROLE_RETIRED] / divisor;
	mPopulationGrowth[PersonRole::ROLE_RETIRED] = mPopulationGrowth[PersonRole::ROLE_RETIRED] % divisor;

	mPopulation[PersonRole::ROLE_RETIRED] += retiree;

	/** Workers retire earlier than scientists. */
	if (randomNumber.generate(0, 100) <= 45) { if (mPopulation[PersonRole::ROLE_SCIENTIST] > 0) { mPopulation[PersonRole::ROLE_SCIENTIST] -= retiree; } }
	else { if (mPopulation[PersonRole::ROLE_WORKER] > 0) { mPopulation[PersonRole::ROLE_WORKER] -= retiree; } }
}


void Population::killChildren(int morale, int nurseries)
{
	if (mPopulation[PersonRole::ROLE_CHILD] < 1) { return; }

	mPopulationDeath[PersonRole::ROLE_CHILD] += mPopulation[PersonRole::ROLE_CHILD];

	int divisor = moraleModifierTable[moraleIndex(morale)].mortalityRate + (nurseries * 10);

	int deaths = mPopulationDeath[PersonRole::ROLE_CHILD] / divisor;
	mPopulationDeath[PersonRole::ROLE_CHILD] = mPopulationDeath[PersonRole::ROLE_CHILD] % divisor;

	mPopulation[PersonRole::ROLE_CHILD] -= deaths;
	mDeathCount += deaths;

	if (mPopulation[PersonRole::ROLE_CHILD] <= 0)
	{
		mPopulationDeath[PersonRole::ROLE_CHILD] = 0;
		mPopulationGrowth[PersonRole::ROLE_STUDENT] = 0;
	}
}


void Population::killStudents(int morale, int hospitals)
{
	if (mPopulation[PersonRole::ROLE_STUDENT] < 1) { return; }

	mPopulationDeath[PersonRole::ROLE_STUDENT] += mPopulation[PersonRole::ROLE_STUDENT];

	int divisor = moraleModifierTable[moraleIndex(morale)].mortalityRate + (hospitals * 65);

	int deaths = mPopulationDeath[PersonRole::ROLE_STUDENT] / divisor;
	mPopulationDeath[PersonRole::ROLE_STUDENT] = mPopulationDeath[PersonRole::ROLE_STUDENT] % divisor;

	mPopulation[PersonRole::ROLE_STUDENT] -= deaths;
	mDeathCount += deaths;

	if (mPopulation[PersonRole::ROLE_STUDENT] <= 0)
	{
		mPopulationDeath[PersonRole::ROLE_STUDENT] = 0;
		mPopulationGrowth[PersonRole::ROLE_WORKER] = 0;
	}
}


void Population::killAdults(Population::PersonRole role, int morale, int hospitals)
{
	if (mPopulation[role] < 1) { return; }
		
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
		mDeathCount = size();
		clear();
		return 0;
	}

	int PopulationFed = food * 10;
	if (PopulationFed > size())
	{
		return size() / 10;
	}


	int populationToKill = static_cast<int>((size() - PopulationFed) * mStarveRate);
	if (size() == 1) { populationToKill = 1; }

	for (int i = 0; i < populationToKill; /**/ )
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
	if (randomNumber.generate(0, 100) <= 45) { killAdults(PersonRole::ROLE_SCIENTIST, morale, hospitals); }
	else { killAdults(PersonRole::ROLE_WORKER, morale, hospitals); }

	killAdults(PersonRole::ROLE_RETIRED, morale, hospitals);

	return consumeFood(food);
}
