#include "StructureManager.h"

#include "Constants.h"

#include <algorithm>


StructureManager::StructureManager() :	mDeferInsert(false),
										mChapActive(false)
{}


/**
 * D'tor
 * 
 * Frees memory for all managed Structures.
 * 
 * \warning	Any remaining pointers to Structures that have been managed
 *			by the StructureManager will be invalid.
 */
StructureManager::~StructureManager()
{
	if (mStructureList.empty())
		return;

	for (auto it = mStructureList.begin(); it != mStructureList.end(); ++it)
		it->second->deleteThing();

	mStructureList.clear();
}


void StructureManager::update()
{
	updateStructures();
}


void StructureManager::processResources(ResourcePool& _r)
{
	processResourcesOut(_r);
	processResourcesIn(_r);

	updateFactories();
}


void StructureManager::updateStructures()
{
	mChapActive = false;

	/* Some structures can generate others (like the seed lander) so we need to make sure that we're
	not inserting new structures while we're in the middle of iterating through the current list. */
	setDeferredFlag(true);
	auto struct_it = mStructureList.begin();
	while (struct_it != mStructureList.end())
	{
		struct_it->first->update();
		struct_it->second->connected(false);	// We iterate through this list on every turn; save some time and
												// reset this flag now so we don't have to do a second loop.

		// FIXME:	Naive approach?
		if (struct_it->first->providesCHAP() && struct_it->first->operational())
			mChapActive = true;



		// Clean up any Structures that are dead.
		if (struct_it->first->dead())
		{
			struct_it->second->deleteThing();
			struct_it = mStructureList.erase(struct_it);
		}
		else
			++struct_it;
	}
	setDeferredFlag(false);
	copyDeferred();
}


/**
 * Iterates through all Structures and provides input resources disabling Structures
 * that don't get the resources they need.
 */
void StructureManager::processResourcesIn(ResourcePool& _r)
{
	auto struct_it = mStructureList.begin();

	/* FIXME:	These checks are a little too complicated and there
				is code duplication.	*/
	while(struct_it != mStructureList.end())
	{
		if(!struct_it->first->isIdle() && !struct_it->first->underConstruction())
		{
			if(struct_it->first->enoughResourcesAvailable(_r) && struct_it->second->connected() && !struct_it->first->destroyed())
			{

				// FIXME: copy paste code, better way to do this.
				if (struct_it->first->requiresCHAP() && !mChapActive)
				{
					struct_it->first->disable();
				}
				else
				{
					struct_it->first->enable();
					_r -= struct_it->first->resourcesIn();
				}
			}
			else
			{
				if(!struct_it->first->selfSustained())
				{
					if(!struct_it->first->destroyed())
						struct_it->first->disable();
				}
			}
		}
		++struct_it;
	}
}


void StructureManager::processResourcesOut(ResourcePool& _r)
{
	// RESOURCES OUT
	ResourcePool out;
	auto struct_it = mStructureList.begin();
	while (struct_it != mStructureList.end())
	{
		if (struct_it->first->operational())
			out += struct_it->first->resourcesOut();

		++struct_it;
	}

	// Adjust resources
	_r += out;
	_r.energy(out.energy()); // energy is not cumulative
}


void StructureManager::updateFactories()
{
	for (size_t i = 0; i < mFactoryList.size(); ++i)
	{
		static_cast<Factory*>(mFactoryList[i])->updateProduction();
	}

}


void StructureManager::copyDeferred()
{
	// could just use std::vector.insert() but addStructure()
	// performs a number of checks that vector.insert() doesn't.
	auto it = mDeferredList.begin();
	while(it != mDeferredList.end())
	{
		addStructure(it->first, it->second, true);
		++it;
	}

	mDeferredList.clear();
}


/**
 * Adds a new Structure to the StructureManager.
 * 
 * \return	True if added successfully. False if the structure is a duplicated.
 * 
 * \param	Call tile method that frees memory or not.
 */
bool StructureManager::addStructure(Structure* st, Tile* t, bool clear)
{
	if(!t)
		return false;

	/// We're in the process of updating structures so defer adding until the updates are finished.
	if(mDeferInsert)
	{
		addToList(mDeferredList, st, t);
		return false;
	}

	for (auto it = mStructureList.begin(); it != mStructureList.end(); ++it)
	{
		if(it->first == st)
			throw Exception(0, "Duplicate Structure!", "StructureManager::addStructure(): Attempting to add a Structure that's already managed.");
	}

	if(!clear)
		t->removeThing();

	addToList(mStructureList, st, t);
	t->pushThing(st);
	t->thingIsStructure(true);

	if (st->isFactory())
	{
		mFactoryList.push_back(st);
	}

	return true;
}


/**
 * Adds a Structure to a references list.
 * 
 * \param _sm	Reference to a StructureMap list.
 * \param _st	Pointer to a Structure. TODO: Note assumptions about ownership of this pointer.
 * \param _t	Pointer to a Tile. NOTE: Pointer not owned by StructureManager -- memory managed by TileMap.
 * 
 * \note	Sorts the list by Structure Priority in ascending order. Higher priority structures (like the CC)
 *			will appear at the beginning of the list.
 */
void StructureManager::addToList(StructureMap& _sm, Structure* _st, Tile* _t)
{
	_sm.push_back(StructureTilePair(_st, _t));
	sort(mStructureList.begin(), mStructureList.end(), [](const StructureTilePair& lhs, const StructureTilePair& rhs) { return lhs.first->priority() > rhs.first->priority(); });
}


/**
 * Removes a Structure from the StructureManager.
 *
 * \return	True if removed successfully. False if the structure is not found.
 */
bool StructureManager::removeStructure(Structure* st)
{
	for (auto it = mStructureList.begin(); it != mStructureList.end(); ++it)
	{
		if (it->first == st)
		{
			if (it->first->isFactory())
			{
				auto factIt = find(mFactoryList.begin(), mFactoryList.end(), st);
				if (factIt != mFactoryList.end())
					mFactoryList.erase(factIt);
			}
			mStructureList.erase(it);
			return true;
		}
	}

	// If we hit this point we're calling the remove function on a structure that was never added to the manager which == BAD
	throw Exception(0, "Rogue Structure!", "StructureManager::removeStructure(): Called with a pointer to a Structure that is not managed!");
	return false;
}


/**
 * Resets the 'connected' flag on all structures in the primary structure list.
 */
void StructureManager::disconnectAll()
{
	for (auto st_it = mStructureList.begin(); st_it != mStructureList.end(); ++st_it)
		st_it->second->connected(false);
}


/**
 * Outputs the primary structure list.
 * 
 * Debug aid to demonstrate structure priority sorting is correct.
 */
void StructureManager::printSortedList()
{
	cout << endl;
	for (size_t i = 0; i < mStructureList.size(); ++i)
		cout << mStructureList[i].first->name() << endl;
	cout << endl;
}