#include "PopulationPool.h"

#include "NAS2D/NAS2D.h"


PopulationPool::PopulationPool() : mScientistsUsed(0), mWorkersUsed(0), mPopulation(nullptr)
{}


PopulationPool::~PopulationPool()
{}


/**
 * Sets a pointer to a Population object.
 * 
 * \note	Note that PopulationPool expects a valid object and does no checking
 *			for invalid states.
 */
void PopulationPool::population(Population* pop)
{
	mPopulation = pop;
}


/**
 * Gets the amount of population available for a given role.
 */
int PopulationPool::populationAvailable(Population::PersonRole _role)
{
	#ifdef _DEBUG // only care if we're in debug mode, fail silently in release modes
	if (_role == Population::ROLE_CHILD || _role == Population::ROLE_STUDENT || _role == Population::ROLE_RETIRED)
		throw NAS2D::Exception(0, "Unhandled Role", "PopulationPool::populationAvailable(): Checking a population role that is not handled by the PopulationPool.");
	#endif

	int employed = 0;

	_role == Population::ROLE_SCIENTIST ? employed = scientistsEmployed() : employed = populationEmployed();

	return mPopulation->size(_role) - employed;
}


/**
 * Gets wheather the specified amount of a particular population role is available.
 * 
 * \returns	True if available is the same or greater than what is being asked for. False otherwise.
 */
bool PopulationPool::enoughPopulationAvailable(Population::PersonRole _role, int _amount)
{
	#ifdef _DEBUG // only care if we're in debug mode, fail silently in release modes
	if (_role == Population::ROLE_CHILD || _role == Population::ROLE_STUDENT || _role == Population::ROLE_RETIRED)
		throw NAS2D::Exception(0, "Unhandled Role", "PopulationPool::populationAvailable(): Checking a population role that is not handled by the PopulationPool.");
	#endif

	return populationAvailable(_role) >= _amount;
}


/**
 * Marks a given amount of the population as set.
 * 
 * \warning	Does not check if the requested amount of population exists. User is required to perform this check.
 * 
 * \warning	Will throw an exception if any role other than Population::ROLE_SCIENTIST or Population::ROLE_WORKER is specified.
 */
void PopulationPool::usePopulation(Population::PersonRole _role, int _amount)
{
	#ifdef _DEBUG // only care if we're in debug mode, fail silently in release modes
	if (_role == Population::ROLE_CHILD || _role == Population::ROLE_STUDENT || _role == Population::ROLE_RETIRED)
		throw NAS2D::Exception(0, "Unhandled Role", "PopulationPool::usePopulation(): Requested a population role that is not handled by the PopulationPool.");
	#endif

	_role == Population::ROLE_SCIENTIST ? mScientistsUsed += _amount : mWorkersUsed += _amount;
}


/**
 * Resets used population counts to 0.
 */
void PopulationPool::clear()
{
	mScientistsUsed = 0;
	mWorkersUsed = 0;
}


int PopulationPool::scientistsEmployed()
{
	return mScientistsUsed;
}


int PopulationPool::workersEmployed()
{
	return mWorkersUsed;
}


int PopulationPool::populationEmployed()
{
	return scientistsEmployed() + workersEmployed();
}
