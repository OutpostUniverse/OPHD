// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Population.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <random>


const int STUDENT_TO_SCIENTIST_RATE = 35;

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
	return std::clamp(morale, 1u, 999u) / 200;
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

	mModifiers[0] = MoraleModifier{50, 50, 110, 80};  // Excellent
	mModifiers[1] = MoraleModifier{25, 25, 90, 75};   // Good
	mModifiers[2] = MoraleModifier{0, 0, 60, 40};     // Fair
	mModifiers[3] = MoraleModifier{-25, -25, 40, 20}; // Poor
	mModifiers[4] = MoraleModifier{-50, -50, 20, 10}; // Terrible
}


/**
 * Clears entire population and frees all associated resources.
 */
void Population::clear()
{
	clearPopulationList(PersonRole::ROLE_CHILD);
	clearPopulationList(PersonRole::ROLE_STUDENT);
	clearPopulationList(PersonRole::ROLE_WORKER);
	clearPopulationList(PersonRole::ROLE_SCIENTIST);
	clearPopulationList(PersonRole::ROLE_RETIRED);
}


/**
 * Clears a specified segment of the population and frees all resources
 * associated with it.
 */
void Population::clearPopulationList(PersonRole _role)
{
	mPopulation[static_cast<std::size_t>(_role)] = 0;
}


/**
 * Populates a given population with a specified number of colonists given some base values.
 *
 * \param	role		Segment of the population to populate.
 * \param	count		Base age in months of the population to populated.
 */
void Population::addPopulation(PersonRole role, uint32_t count)
{
	mPopulation[static_cast<std::size_t>(role)] += count;
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
	return mPopulation[static_cast<std::size_t>(_pr)];
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
	if (mPopulation[static_cast<std::size_t>(PersonRole::ROLE_SCIENTIST)] + mPopulation[static_cast<std::size_t>(PersonRole::ROLE_WORKER)] > mPopulation[static_cast<std::size_t>(PersonRole::ROLE_CHILD)])
	{
		mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] += mPopulation[static_cast<std::size_t>(PersonRole::ROLE_SCIENTIST)] / 4 + mPopulation[static_cast<std::size_t>(PersonRole::ROLE_WORKER)] / 2;

		int divisor = mModifiers[moraleIndex(morale)].fertilityRate;

		int newChildren = mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] / divisor;
		mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] = mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] % divisor;

		mPopulation[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] += newChildren;
		mBirthCount = newChildren;
	}
}


void Population::spawn_students()
{
	if (mPopulation[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] > 0)
	{
		mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] += mPopulation[static_cast<std::size_t>(PersonRole::ROLE_CHILD)];

		int divisor = mPopulation[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] + mPopulation[static_cast<std::size_t>(PersonRole::ROLE_WORKER)] + mPopulation[static_cast<std::size_t>(PersonRole::ROLE_SCIENTIST)] + mPopulation[static_cast<std::size_t>(PersonRole::ROLE_CHILD)];
		if (divisor <= STUDENT_TO_ADULT_BASE) { divisor = STUDENT_TO_ADULT_BASE; }
		divisor = ((divisor / 40) * 3 + 16) * 4;

		int newStudents = mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] / divisor;
		mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] = mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] % divisor;

		mPopulation[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] += newStudents;
		mPopulation[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] -= newStudents;
	}
}


void Population::spawn_adults(int universities)
{
	//-- New Adults --//
	if (mPopulation[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] > 0)
	{
		mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_WORKER)] += mPopulation[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)];

		int divisor = mPopulation[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] + mPopulation[static_cast<std::size_t>(PersonRole::ROLE_WORKER)] + mPopulation[static_cast<std::size_t>(PersonRole::ROLE_SCIENTIST)] + mPopulation[static_cast<std::size_t>(PersonRole::ROLE_CHILD)];
		if(divisor <= STUDENT_TO_ADULT_BASE) { divisor = STUDENT_TO_ADULT_BASE; }

		divisor = ((divisor / 40) * 3 + 45) * 4;

		int newAdult = mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_WORKER)] / divisor;
		mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_WORKER)] = mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_WORKER)] % divisor;

		// account for universities
		if(universities > 0 && random_0_100() <= STUDENT_TO_SCIENTIST_RATE)
		{
			mPopulation[static_cast<std::size_t>(PersonRole::ROLE_SCIENTIST)] += newAdult;
		} else
		{
			mPopulation[static_cast<std::size_t>(PersonRole::ROLE_WORKER)] += newAdult;
		}

		mPopulation[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] -= newAdult;
	}
}


void Population::spawn_retiree()
{
	int total_adults = mPopulation[static_cast<std::size_t>(PersonRole::ROLE_WORKER)] + mPopulation[static_cast<std::size_t>(PersonRole::ROLE_SCIENTIST)];
	if (total_adults > 0)
	{
		mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_RETIRED)] += total_adults / 10;

		int divisor = total_adults;
		if (divisor <= ADULT_TO_RETIREE_BASE) { divisor = ADULT_TO_RETIREE_BASE; }

		divisor = ((divisor / 40) * 3 + 40) * 4;

		int retiree = mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_RETIRED)] / divisor;
		mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_RETIRED)] = mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_RETIRED)] % divisor;

		mPopulation[static_cast<std::size_t>(PersonRole::ROLE_RETIRED)] += retiree;

		/** Workers retire earlier than scientists. */
		if (random_0_100() <= 45) { if (mPopulation[static_cast<std::size_t>(PersonRole::ROLE_SCIENTIST)] > 0) { mPopulation[static_cast<std::size_t>(PersonRole::ROLE_SCIENTIST)] -= retiree; } }
		else { if (mPopulation[static_cast<std::size_t>(PersonRole::ROLE_WORKER)] > 0) { mPopulation[static_cast<std::size_t>(PersonRole::ROLE_WORKER)] -= retiree; } }
	}
}


void Population::kill_children(int morale, int nurseries)
{
	if (mPopulation[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] > 0)
	{
		mPopulationDeath[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] = mPopulation[static_cast<std::size_t>(PersonRole::ROLE_CHILD)];

		int divisor = mModifiers[moraleIndex(morale)].mortalityRate + (nurseries * 10);

		int deaths = mPopulationDeath[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] / divisor;
		mPopulationDeath[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] = mPopulationDeath[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] % divisor;

		mPopulation[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] -= deaths;
		mDeathCount += deaths;

		if(mPopulation[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] <= 0)
		{
			mPopulationDeath[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] = 0;
			mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] = 0;
		}
	}
}


void Population::kill_students(int morale, int hospitals)
{
	if (mPopulation[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] > 0)
	{
		mPopulationDeath[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] = mPopulation[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)];

		int divisor = mModifiers[moraleIndex(morale)].mortalityRate + (hospitals * 65);

		int deaths = mPopulationDeath[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] / divisor;
		mPopulationDeath[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] = mPopulationDeath[static_cast<std::size_t>(PersonRole::ROLE_CHILD)] % divisor;

		mPopulation[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] -= deaths;
		mDeathCount += deaths;

		if (mPopulation[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] <= 0)
		{
			mPopulationDeath[static_cast<std::size_t>(PersonRole::ROLE_STUDENT)] = 0;
			mPopulationGrowth[static_cast<std::size_t>(PersonRole::ROLE_WORKER)] = 0;
		}
	}
}


void Population::kill_adults(Population::PersonRole role, int morale, int hospitals)
{
	// Worker Deaths
	if (mPopulation[static_cast<std::size_t>(role)] > 0)
	{
		mPopulationDeath[static_cast<std::size_t>(role)] += mPopulation[static_cast<std::size_t>(role)];
		int divisor = mModifiers[moraleIndex(morale)].mortalityRate + 250 + (hospitals * 60);

		int deaths = mPopulationDeath[static_cast<std::size_t>(role)] / divisor;
		mPopulationDeath[static_cast<std::size_t>(role)] = mPopulationDeath[static_cast<std::size_t>(role)] % divisor;

		mPopulation[static_cast<std::size_t>(role)] -= deaths;
		mDeathCount += deaths;

		if (mPopulation[static_cast<std::size_t>(role)] == 0)
		{
			mPopulationDeath[static_cast<std::size_t>(role)] = 0;
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

	for (uint32_t i = 0; i < population_to_kill; /**/ )
	{
		int role_idx = i % 5;

		int counter = 0;
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
	if (random_0_100() <= 45) { kill_adults(PersonRole::ROLE_SCIENTIST, morale, hospitals); }
	else { kill_adults(PersonRole::ROLE_WORKER, morale, hospitals); }

	kill_adults(PersonRole::ROLE_RETIRED, morale, hospitals);

	return consume_food(food);
}
