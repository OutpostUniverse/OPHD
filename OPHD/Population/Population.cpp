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
	mPopulation.clear();
	mPopulationGrowth.clear();
	mPopulationDeath.clear();
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


int Population::adults() const
{
	return mPopulation[PopulationTable::Role::Student] + mPopulation[PopulationTable::Role::Worker] + mPopulation[PopulationTable::Role::Scientist] + mPopulation[PopulationTable::Role::Retired];
}

/**
 * Population check for new children.
 * 
 * \todo	Account for nurseries when implemented.
 */
void Population::spawn_children(int morale, int residences, int nurseries)
{
	if (residences < 1 && nurseries < 1) { return; }

	// This should be adjusted to maybe two or three kids per couple to allow for higher growth rates.
	if (mPopulation[PopulationTable::Role::Scientist] + mPopulation[PopulationTable::Role::Worker] > mPopulation[PopulationTable::Role::Child])
	{
		mPopulationGrowth[PopulationTable::Role::Child] += mPopulation[PopulationTable::Role::Scientist] / 4 + mPopulation[PopulationTable::Role::Worker] / 2;

		int divisor = moraleModifierTable[moraleIndex(morale)].fertilityRate;

		int newChildren = mPopulationGrowth[PopulationTable::Role::Child] / divisor;
		mPopulationGrowth[PopulationTable::Role::Child] = mPopulationGrowth[PopulationTable::Role::Child] % divisor;

		mPopulation[PopulationTable::Role::Child] += newChildren;
		mBirthCount = newChildren;
	}
}


void Population::spawn_students()
{
	if (mPopulation[PopulationTable::Role::Child] > 0)
	{
		mPopulationGrowth[PopulationTable::Role::Student] += mPopulation[PopulationTable::Role::Child];

		int divisor = std::max(adults(), studentToAdultBase);
		divisor = ((divisor / 40) * 3 + 16) * 4;

		int newStudents = mPopulationGrowth[PopulationTable::Role::Student] / divisor;
		mPopulationGrowth[PopulationTable::Role::Student] = mPopulationGrowth[PopulationTable::Role::Student] % divisor;

		mPopulation[PopulationTable::Role::Student] += newStudents;
		mPopulation[PopulationTable::Role::Child] -= newStudents;
	}
}


void Population::spawn_adults(int universities)
{
	//-- New Adults --//
	if (mPopulation[PopulationTable::Role::Student] > 0)
	{
		mPopulationGrowth[PopulationTable::Role::Worker] += mPopulation[PopulationTable::Role::Student];

		int divisor = std::max(adults(), studentToAdultBase);
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
}


void Population::spawn_retiree()
{
	int total_adults = mPopulation[PopulationTable::Role::Worker] + mPopulation[PopulationTable::Role::Scientist];
	if (total_adults > 0)
	{
		mPopulationGrowth[PopulationTable::Role::Retired] += total_adults / 10;

		int divisor = std::max(total_adults, adultToRetireeBase);

		divisor = ((divisor / 40) * 3 + 40) * 4;

		int retiree = mPopulationGrowth[PopulationTable::Role::Retired] / divisor;
		mPopulationGrowth[PopulationTable::Role::Retired] = mPopulationGrowth[PopulationTable::Role::Retired] % divisor;

		mPopulation[PopulationTable::Role::Retired] += retiree;

		/** Workers retire earlier than scientists. */
		if (randomNumber.generate(0, 100) <= 45) { if (mPopulation[PopulationTable::Role::Scientist] > 0) { mPopulation[PopulationTable::Role::Scientist] -= retiree; } }
		else { if (mPopulation[PopulationTable::Role::Worker] > 0) { mPopulation[PopulationTable::Role::Worker] -= retiree; } }
	}
}


void Population::kill_children(int morale, int nurseries)
{
	if (mPopulation[PopulationTable::Role::Child] > 0)
	{
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
}


void Population::kill_students(int morale, int hospitals)
{
	if (mPopulation[PopulationTable::Role::Child] > 0)
	{
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
}


void Population::kill_adults(PopulationTable::Role role, int morale, int hospitals)
{
	// Worker Deaths
	if (mPopulation[role] > 0)
	{
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
}


/**
 * Determine how much food should be consumed and kill off any population that
 * starves.
 *
 * \return	Actual amount of food consumed.
 */
int Population::consume_food(int food)
{
	// If there's no food kill everybody (humans can survive up to 21 days without food, one turn == minimum 28 days)
	if (food == 0)
	{
		mDeathCount = mPopulation.size();
		clear();
		return 0;
	}

	int population_fed = food * 10;
	if (population_fed > mPopulation.size())
	{
		return mPopulation.size() / 10;
	}


	int population_to_kill = static_cast<int>((mPopulation.size() - population_fed) * mStarveRate);
	if (mPopulation.size() == 1) { population_to_kill = 1; }

	for (int i = 0; i < population_to_kill; /**/ )
	{
		std::size_t role_idx = i % 5;

		std::size_t counter = 0;
		for (;;)
		{
			role_idx = role_idx + counter;
			if (role_idx > 4) { role_idx = 0; }

			if (mPopulation[role_idx] > 0)
			{
				break;
			}

			++counter;
			if (counter > 4) { counter = 0; }
		}

		--mPopulation[role_idx];
		++i;
	}

	mDeathCount = population_to_kill;

	// actual amount of food used for the fed part of the population.
	return population_fed / 10;
}


/**
 * \return	Actual amount of food consumed.
 */
int Population::update(int morale, int food, int residences, int universities, int nurseries, int hospitals)
{
	mBirthCount = 0;
	mDeathCount = 0;

	spawn_children(morale, residences, nurseries);
	spawn_students();
	spawn_adults(universities);
	spawn_retiree();

	kill_children(morale, nurseries);
	kill_students(morale, hospitals);

	// Workers will die more often than scientists.
	if (randomNumber.generate(0, 100) <= 45) { kill_adults(PopulationTable::Role::Scientist, morale, hospitals); }
	else { kill_adults(PopulationTable::Role::Worker, morale, hospitals); }

	kill_adults(PopulationTable::Role::Retired, morale, hospitals);

	return consume_food(food);
}
