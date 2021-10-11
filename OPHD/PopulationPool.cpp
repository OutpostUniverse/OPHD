#include "PopulationPool.h"

#include <string>
#include <stdexcept>


/**
 * Sets a pointer to a Population object.
 * 
 * \note	PopulationPool expects a valid object and does no checking
 *			for invalid states.
 */
void PopulationPool::population(Population* pop)
{
	mPopulation = pop;
}


int PopulationPool::availableWorkers()
{
	return mPopulation->size(PopulationTable::Role::Worker) - workersEmployed();
}


int PopulationPool::availableScientists()
{
	return mPopulation->size(PopulationTable::Role::Scientist) - scientistsEmployed();
}


bool PopulationPool::usePopulation(PopulationRequirements populationRequirements)
{
	const auto [workersRequired, scientistsRequired] = populationRequirements;

	return usePopulation(PopulationTable::Role::Worker, workersRequired) &&
		usePopulation(PopulationTable::Role::Scientist, scientistsRequired);
}


/**
 * Marks a given amount of the population as set.
 * 
 * \warning	Will throw an exception if any role other than PopulationTable::Role::Scientist or PopulationTable::Role::Worker is specified.
 * 
 * \return	Returns true if population was assigned. False if insufficient population.
 */
bool PopulationPool::usePopulation(PopulationTable::Role role, int amount)
{
	int scientistsAvailable = mPopulation->size(PopulationTable::Role::Scientist) - (mScientistsAsWorkers + mScientistsUsed);
	int workersAvailable = mPopulation->size(PopulationTable::Role::Worker) - mWorkersUsed;


	if (role == PopulationTable::Role::Scientist)
	{
		if (amount <= scientistsAvailable)
		{
			mScientistsUsed += amount;
			return true;
		}
	}
	else if (role == PopulationTable::Role::Worker)
	{
		if (amount <= workersAvailable + scientistsAvailable)
		{
			if (amount <= workersAvailable)
			{
				mWorkersUsed += amount;
				return true;
			}

			int remainder = amount - workersAvailable;
			mWorkersUsed += amount - remainder;
			mScientistsAsWorkers += remainder;
			return true;
		}
	}

	return false;
}


/**
 * Resets used population counts to 0.
 */
void PopulationPool::clear()
{
	mScientistsAsWorkers = 0;
	mScientistsUsed = 0;
	mWorkersUsed = 0;
}


/**
 * Amount of Scientists employed as Workers.
 */
int PopulationPool::scientistsAsWorkers()
{
	return mScientistsAsWorkers;
}


/**
 * Amount of Scientists currently employed.
 */
int PopulationPool::scientistsEmployed()
{
	return mScientistsUsed;
}


/**
 * Amount of Workers currently employed.
 */
int PopulationPool::workersEmployed()
{
	return mWorkersUsed;
}


/**
 * Amount of population currently employed.
 */
int PopulationPool::populationEmployed()
{
	return scientistsEmployed() + scientistsAsWorkers() + workersEmployed();
}
