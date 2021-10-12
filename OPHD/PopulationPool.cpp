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
	return mPopulation->getPopulations().size(PopulationTable::Role::Worker) - workersEmployed();
}


int PopulationPool::availableScientists()
{
	return mPopulation->getPopulations().size(PopulationTable::Role::Scientist) - scientistsEmployed();
}


bool PopulationPool::usePopulation(PopulationRequirements populationRequirements)
{
	const auto [workersRequired, scientistsRequired] = populationRequirements;

	const auto population = mPopulation->getPopulations();
	const auto scientistsAvailable = population.size(PopulationTable::Role::Scientist) - (mScientistsAsWorkers + mScientistsUsed);
	const auto workersAvailable = population.size(PopulationTable::Role::Worker) - mWorkersUsed;

	if ((scientistsRequired > scientistsAvailable) || (workersRequired > workersAvailable + (scientistsAvailable - scientistsRequired)))
	{
		return false;
	}

	mScientistsUsed += scientistsRequired;
	const auto workersUsed = std::min(workersRequired, workersAvailable);
	mWorkersUsed += workersUsed;
	mScientistsAsWorkers += workersRequired - workersUsed;

	return true;
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
