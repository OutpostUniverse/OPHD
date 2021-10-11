#include "PopulationPool.h"

#include <string>
#include <stdexcept>


namespace
{
	void BasicCheck(PopulationTable::Role role);
}


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


/**
 * Marks a given amount of the population as set.
 * 
 * \warning	Will throw an exception if any role other than PopulationTable::Role::Scientist or PopulationTable::Role::Worker is specified.
 * 
 * \return	Returns true if population was assigned. False if insufficient population.
 */
bool PopulationPool::usePopulation(PopulationTable::Role role, int amount)
{
	BasicCheck(role);
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


// ===============================================================================

namespace
{
	/**
	 * Does a basic check to ensure that we're only trying to pull population that can be employed.
	 *
	 * Generally speaking the only 'workable' population is for Workers and Scientists. Children, Students
	 * and Retirees won't be pulled for labor/research so attempting to pull this should be considered
	 * a mistake and should fail very loudly. In this case throws a std::runtime_error.
	 *
	 * In the future this may change but for now this is almost strictly a debugging aid. This failure
	 * would indicate a very significant problem with the calling code.
	 *
	 * \throws	std::runtime_exception if Child/Student/Retired is asked for.
	 */
	void BasicCheck(PopulationTable::Role role)
	{
		if (role == PopulationTable::Role::Child || role == PopulationTable::Role::Student || role == PopulationTable::Role::Retired)
		{
			std::string roleTypeName;
			switch (role)
			{
			case PopulationTable::Role::Child: roleTypeName = "PopulationTable::Role::Child"; break;
			case PopulationTable::Role::Student: roleTypeName = "PopulationTable::Role::Student"; break;
			case PopulationTable::Role::Retired: roleTypeName = "PopulationTable::Role::Retired"; break;
			default: break;
			}

			throw std::runtime_error("PopulationPool::BasicCheck(): Invalid population role specified (" + roleTypeName + ").");
		}
	}
}
