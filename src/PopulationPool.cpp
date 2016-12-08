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
 * Gets wheather the specified amount of a particular population role is available.
 * 
 * \returns	True if available is the same or greater than what is being asked for. False otherwise.
 */
bool PopulationPool::populationAvailable(Population::PersonRole _role, int _amount)
{
	return _amount <= mPopulation->size(_role);
}


/**
 * Marks a given amount of the population as set.
 * 
 * \note	If the amount asked for exceeds what's available, an exception will be thrown. This is for debug purposes.
 * 
 * \note	Does not check for 
 */
void PopulationPool::usePopulation(Population::PersonRole _role, int _amount)
{
	// Basic checks
	switch (_role)
	{
	case Population::ROLE_SCIENTIST:
		 
		break;
	case Population::ROLE_WORKER:
		break;
	default:
		#ifdef _DEBUG // only care if we're in debug mode, fail silently in release modes
		if (_role == Population::ROLE_CHILD || _role == Population::ROLE_STUDENT || _role == Population::ROLE_RETIRED)
			throw NAS2D::Exception(0, "Unhandled Role", "PopulationPool::usePopulation(): Requested a population role that is not handled by the PopulationPool.");
		#endif
		break;
	}


}


/**
 * Resets used population counts to 0.
 */
void PopulationPool::clear()
{
	mScientistsUsed = 0;
	mWorkersUsed = 0;
}