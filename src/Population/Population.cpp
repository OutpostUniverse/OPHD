#include "Population.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <random>

#include <NAS2D/NAS2D.h>


const int STUDENT_TO_SCIENTIST_RATE = 35;

const int CHILD_TO_STUDENT_BASE = 120;
const int STUDENT_TO_ADULT_BASE = 190;
const int ADULT_TO_RETIREE_BASE = 2000;


std::default_random_engine pop_generator;
std::uniform_int_distribution<int> pop_distribution(0, 100);

auto random_0_100 = std::bind(pop_distribution, pop_generator);

/**
 * Convenience function to cast a MoraleLevel enumerator
 * into an array index.
 */
int moraleIndex(uint32_t morale)
{
	return NAS2D::clamp(morale, 1, 999) / 200;
}



/**
 * C'tor
 */
Population::Population() : mBirthCount(0), mDeathCount(0), mStarveRate(0.5f)
{
	init();
}


/**
 * D'tor
 */
Population::~Population()
{
	clear();
}


void Population::init()
{
	mPopulation.fill(0);
	mPopulationGrowth.fill(0);
	mPopulationDeath.fill(0);

	mModifiers[0] = MoraleModifier(50, 50, 110, 80);	// Excellent
	mModifiers[1] = MoraleModifier(25, 25, 90, 75);		// Good
	mModifiers[2] = MoraleModifier(0, 0, 60, 40);		// Fair
	mModifiers[3] = MoraleModifier(-25, -25, 40, 20);	// Poor
	mModifiers[4] = MoraleModifier(-50, -50, 20, 10);	// Terrible
}


/**
 * Clears entire population and frees all associated resources.
 */
void Population::clear()
{
	clearPopulationList(ROLE_CHILD);
	clearPopulationList(ROLE_STUDENT);
	clearPopulationList(ROLE_WORKER);
	clearPopulationList(ROLE_SCIENTIST);
	clearPopulationList(ROLE_RETIRED);
}


/**
 * Clears a specified segment of the population and frees all resources
 * associated with it.
 */
void Population::clearPopulationList(PersonRole _role)
{
	mPopulation[_role] = 0;
}


/**
 * Populates a given population with a specified number of colonists given some base values.
 *
 * \param	role		Segment of the population to populate.
 * \param	count		Base age in months of the population to populated.
 */
void Population::addPopulation(PersonRole role, uint32_t count)
{
	mPopulation[role] += count;
}


/**
 * Gets the size of the entire population.
 */
int Population::size()
{
	uint32_t count = 0;
	for (size_t i = 0; i < mPopulation.size(); ++i)
	{
		count += mPopulation[i];
	}

	return count;
}


/**
 * Gets the size of a specific segment of the population.
 */
int Population::size(PersonRole _pr)
{
	return mPopulation[_pr];
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
	if (mPopulation[ROLE_SCIENTIST] + mPopulation[ROLE_WORKER] > mPopulation[ROLE_CHILD])
	{
		mPopulationGrowth[ROLE_CHILD] += mPopulation[ROLE_SCIENTIST] / 4 + mPopulation[ROLE_WORKER] / 2;

		int divisor = mModifiers[moraleIndex(morale)].fertilityRate;

		int newChildren = mPopulationGrowth[ROLE_CHILD] / divisor;
		mPopulationGrowth[ROLE_CHILD] = mPopulationGrowth[ROLE_CHILD] % divisor;

		mPopulation[ROLE_CHILD] += newChildren;
		mBirthCount = newChildren;
	}
}


void Population::spawn_students()
{
	if (mPopulation[ROLE_CHILD] > 0)
	{
		mPopulationGrowth[ROLE_STUDENT] += mPopulation[ROLE_CHILD];

		int divisor = mPopulation[ROLE_STUDENT] + mPopulation[ROLE_WORKER] + mPopulation[ROLE_SCIENTIST] + mPopulation[ROLE_CHILD];
		if (divisor <= STUDENT_TO_ADULT_BASE) { divisor = STUDENT_TO_ADULT_BASE; }
		divisor = ((divisor / 40) * 3 + 16) * 4;

		int newStudents = mPopulationGrowth[ROLE_STUDENT] / divisor;
		mPopulationGrowth[ROLE_STUDENT] = mPopulationGrowth[ROLE_STUDENT] % divisor;

		mPopulation[ROLE_STUDENT] += newStudents;
		mPopulation[ROLE_CHILD] -= newStudents;
	}
}


void Population::spawn_adults(int universities)
{
	//-- New Adults --//
	if (mPopulation[ROLE_STUDENT] > 0)
	{
		mPopulationGrowth[ROLE_WORKER] += mPopulation[ROLE_STUDENT];

		int divisor = mPopulation[ROLE_STUDENT] + mPopulation[ROLE_WORKER] + mPopulation[ROLE_SCIENTIST] + mPopulation[ROLE_CHILD];
		if (divisor <= STUDENT_TO_ADULT_BASE) { divisor = STUDENT_TO_ADULT_BASE; }

		divisor = ((divisor / 40) * 3 + 45) * 4;

		int newAdult = mPopulationGrowth[ROLE_WORKER] / divisor;
		mPopulationGrowth[ROLE_WORKER] = mPopulationGrowth[ROLE_WORKER] % divisor;

		// account for universities
		if (universities > 0 && random_0_100() <= STUDENT_TO_SCIENTIST_RATE)
		{
			mPopulation[ROLE_SCIENTIST] += newAdult;
		}
		else
		{
			mPopulation[ROLE_WORKER] += newAdult;
		}

		mPopulation[ROLE_STUDENT] -= newAdult;
	}
}


void Population::spawn_retiree()
{
	int total_adults = mPopulation[ROLE_WORKER] + mPopulation[ROLE_SCIENTIST];
	if (total_adults > 0)
	{
		mPopulationGrowth[ROLE_RETIRED] += total_adults / 10;

		int divisor = total_adults;
		if (divisor <= ADULT_TO_RETIREE_BASE) { divisor = ADULT_TO_RETIREE_BASE; }

		divisor = ((divisor / 40) * 3 + 40) * 4;

		int retiree = mPopulationGrowth[ROLE_RETIRED] / divisor;
		mPopulationGrowth[ROLE_RETIRED] = mPopulationGrowth[ROLE_RETIRED] % divisor;

		mPopulation[ROLE_RETIRED] += retiree;

		/** Workers retire earlier than scientists. */
		if (random_0_100() <= 45) { if (mPopulation[ROLE_SCIENTIST] > 0) { mPopulation[ROLE_SCIENTIST] -= retiree; } }
		else { if (mPopulation[ROLE_WORKER] > 0) { mPopulation[ROLE_WORKER] -= retiree; } }
	}
}


void Population::kill_children(int morale, int nurseries)
{
	if (mPopulation[ROLE_CHILD] > 0)
	{
		mPopulationDeath[ROLE_CHILD] = mPopulation[ROLE_CHILD];

		int divisor = mModifiers[moraleIndex(morale)].mortalityRate + (nurseries * 10);

		int deaths = mPopulationDeath[ROLE_CHILD] / divisor;
		mPopulationDeath[ROLE_CHILD] = mPopulationDeath[ROLE_CHILD] % divisor;

		mPopulation[ROLE_CHILD] -= deaths;
		mDeathCount += deaths;

		if (mPopulation[ROLE_CHILD] <= 0)
		{
			mPopulationDeath[ROLE_CHILD] = 0;
			mPopulationGrowth[ROLE_STUDENT] = 0;
		}
	}
}


void Population::kill_students(int morale, int hospitals)
{
	if (mPopulation[ROLE_CHILD] > 0)
	{
		mPopulationDeath[ROLE_STUDENT] = mPopulation[ROLE_STUDENT];

		int divisor = mModifiers[moraleIndex(morale)].mortalityRate + (hospitals * 65);

		int deaths = mPopulationDeath[ROLE_STUDENT] / divisor;
		mPopulationDeath[ROLE_STUDENT] = mPopulationDeath[ROLE_CHILD] % divisor;

		mPopulation[ROLE_STUDENT] -= deaths;
		mDeathCount += deaths;

		if (mPopulation[ROLE_STUDENT] <= 0)
		{
			mPopulationDeath[ROLE_STUDENT] = 0;
			mPopulationGrowth[ROLE_WORKER] = 0;
		}
	}
}


void Population::kill_adults(Population::PersonRole role, int morale, int hospitals)
{
	// Worker Deaths
	if (mPopulation[role] > 0)
	{
		mPopulationDeath[role] += mPopulation[role];
		int divisor = mModifiers[moraleIndex(morale)].mortalityRate + 250 + (hospitals * 60);

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
uint32_t Population::consume_food(uint32_t food)
{
	// If there's no food kill everybody (humans can survive up to 21 days without food, one turn == minimum 28 days)
	if (food == 0)
	{
		mDeathCount = size();
		clear();
		return 0;
	}

	uint32_t population_fed = food * 10;
	if (population_fed > static_cast<uint32_t>(size()))
	{
		return size() / 10;
	}


	uint32_t population_to_kill = static_cast<int>((size() - population_fed) * mStarveRate);
	if (size() == 1) { population_to_kill = 1; }

	for (int i = 0; i < population_to_kill; /**/ )
	{
		PersonRole role = static_cast<PersonRole>(i % 5);
		if (mPopulation[role] > 0)
		{
			--mPopulation[role];
			++i;
		}
	}

	mDeathCount = population_to_kill;

	// actual amount of population fed.
	return population_fed;
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
	if (random_0_100() <= 45) { kill_adults(ROLE_SCIENTIST, morale, hospitals); }
	else { kill_adults(ROLE_WORKER, morale, hospitals); }

	kill_adults(ROLE_RETIRED, morale, hospitals);

	return consume_food(food);
}
