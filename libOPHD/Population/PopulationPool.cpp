#include "PopulationPool.h"

#include "PopulationModel.h"
#include "PopulationRequirements.h"

#include <algorithm>
#include <string>
#include <stdexcept>


/**
 * Sets a pointer to a PopulationModel object.
 *
 * \note	PopulationPool expects a valid object and does no checking
 *			for invalid states.
 */
void PopulationPool::population(PopulationModel* pop)
{
	mPopulation = pop;
}


int PopulationPool::availableWorkers() const
{
	return mPopulation->getPopulations().worker - workersEmployed();
}


int PopulationPool::availableScientists() const
{
	return mPopulation->getPopulations().scientist - scientistsEmployed();
}


bool PopulationPool::usePopulation(PopulationRequirements populationRequirements)
{
	const auto [workersRequired, scientistsRequired] = populationRequirements;

	const auto& population = mPopulation->getPopulations();
	const auto scientistsAvailable = population.scientist - (mScientistsAsWorkers + mScientistsUsed);
	const auto workersAvailable = population.worker - mWorkersUsed;

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
int PopulationPool::scientistsAsWorkers() const
{
	return mScientistsAsWorkers;
}


/**
 * Amount of Scientists currently employed.
 */
int PopulationPool::scientistsEmployed() const
{
	return mScientistsUsed;
}


/**
 * Amount of Workers currently employed.
 */
int PopulationPool::workersEmployed() const
{
	return mWorkersUsed;
}


/**
 * Amount of population currently employed.
 */
int PopulationPool::populationEmployed() const
{
	return scientistsEmployed() + scientistsAsWorkers() + workersEmployed();
}


int PopulationPool::size() const
{
	return mPopulation->getPopulations().size();
}
