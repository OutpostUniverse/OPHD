#include "PopulationPool.h"

#include <string>
#include <stdexcept>


namespace {
	void BasicCheck(PopulationTable::PersonRole role);
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


/**
 * Gets the amount of population available for a given role.
 */
int PopulationPool::populationAvailable(PopulationTable::PersonRole role)
{
	BasicCheck(role);

	int employed = 0;
	employed = role == PopulationTable::PersonRole::ROLE_SCIENTIST ? scientistsEmployed() : workersEmployed();

	return mPopulation->size(role) - employed;
}


/**
 * Gets wheather the specified amount of a particular population role is available.
 * 
 * \returns	True if available is the same or greater than what is being asked for. False otherwise.
 */
bool PopulationPool::enoughPopulationAvailable(PopulationTable::PersonRole role, int amount)
{
	BasicCheck(role);
	return populationAvailable(role) >= amount;
}


/**
 * Marks a given amount of the population as set.
 * 
 * \warning	Will throw an exception if any role other than Population::PersonRole::ROLE_SCIENTIST or Population::PersonRole::ROLE_WORKER is specified.
 * 
 * \return	Returns true if population was assigned. False if insufficient population.
 */
bool PopulationPool::usePopulation(PopulationTable::PersonRole role, int amount)
{
	BasicCheck(role);
	int scientistsAvailable = mPopulation->size(PopulationTable::PersonRole::ROLE_SCIENTIST) - (mScientistsAsWorkers + mScientistsUsed);
	int workersAvailable = mPopulation->size(PopulationTable::PersonRole::ROLE_WORKER) - mWorkersUsed;


	if (role == PopulationTable::PersonRole::ROLE_SCIENTIST)
	{
		if (amount <= scientistsAvailable)
		{
			mScientistsUsed += amount;
			return true;
		}
	}
	else if (role == PopulationTable::PersonRole::ROLE_WORKER)
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

namespace {
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
	void BasicCheck(PopulationTable::PersonRole role)
	{
		if (role == PopulationTable::PersonRole::ROLE_CHILD || role == PopulationTable::PersonRole::ROLE_STUDENT || role == PopulationTable::PersonRole::ROLE_RETIRED)
		{
			std::string roleTypeName;
			switch (role)
			{
			case PopulationTable::PersonRole::ROLE_CHILD: roleTypeName = "Population::PersonRole::ROLE_CHILD"; break;
			case PopulationTable::PersonRole::ROLE_STUDENT: roleTypeName = "Population::PersonRole::ROLE_STUDENT"; break;
			case PopulationTable::PersonRole::ROLE_RETIRED: roleTypeName = "Population::PersonRole::ROLE_RETIRED"; break;
			default: break;
			}

			throw std::runtime_error("PopulationPool::BasicCheck(): Invalid population role specified (" + roleTypeName + ").");
		}
	}
}
