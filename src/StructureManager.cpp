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
		it->second.tile->deleteThing();

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
		struct_it->second.tile->connected(false);	// We iterate through this list on every turn; save some time and
													// reset this flag now so we don't have to do a second loop.

		// FIXME:	Naive approach?
		if (struct_it->first->providesCHAP() && struct_it->first->enabled())
			mChapActive = true;

		if(struct_it->first->idle())
			struct_it->first->sprite().color(255, 255, 255, 185);
		else
			struct_it->first->sprite().color(255, 255, 255, 255);

		// Clean up any Structures that are dead.
		if (struct_it->first->dead())
		{
			struct_it->second.tile->deleteThing();
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
		if(!struct_it->first->idle() && !struct_it->first->underConstruction())
		{
			if(struct_it->first->enoughResourcesAvailable(_r) && struct_it->second.tile->connected())
			{

				// FIXME: copy paste code, better way to do this.
				if (struct_it->first->requiresCHAP() && !mChapActive)
				{
					struct_it->first->enabled(false);
					struct_it->first->sprite().color(255, 0, 0, 185);
				}
				else
				{
					struct_it->first->enabled(true);
					struct_it->first->sprite().color(255, 255, 255, 255);

					_r -= struct_it->first->resourcesIn();
				}
			}
			else
			{
				if(!struct_it->first->selfSustained())
				{
					struct_it->first->enabled(false);
					struct_it->first->sprite().color(255, 0, 0, 185);
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
		if (!struct_it->first->underConstruction() && !struct_it->first->idle() && struct_it->first->enabled())
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
	StructureMap::iterator it = mDeferredList.begin();
	while(it != mDeferredList.end())
	{
		addStructure(it->first, it->second.tile, it->second.x, it->second.y, it->second.depth, true);
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
bool StructureManager::addStructure(Structure* st, Tile* t, int x, int y, int depth, bool clear)
{
	if(!t)
		return false;

	TilePositionInfo tpi(t, x, y, depth);

	/// We're in the process of updating structures so defer adding until the updates are finished.
	if(mDeferInsert)
	{
		addToList(mDeferredList, st, tpi);
		return false;
	}

	for (auto it = mStructureList.begin(); it != mStructureList.end(); ++it)
	{
		if(it->first == st)
			throw Exception(0, "Duplicate Structure!", "StructureManager::addStructure(): Attempting to add a Structure that's already managed.");
	}

	if(!clear)
		t->removeThing();

	addToList(mStructureList, st, tpi);
	t->pushThing(st);
	t->thingIsStructure(true);

	if (st->isFactory())
	{
		mFactoryList.push_back(st);
	}

	return true;
}


void StructureManager::addToList(StructureMap& _sm, Structure* _st, TilePositionInfo& _tpi)
{
	_sm.push_back(StructureTilePair(_st, _tpi));

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


void StructureManager::disconnectAll()
{
	for (auto st_it = mStructureList.begin(); st_it != mStructureList.end(); ++st_it)
		st_it->second.tile->connected(false);
}


void StructureManager::printSortedList()
{
	cout << endl;
	for (size_t i = 0; i < mStructureList.size(); ++i)
		cout << mStructureList[i].first->name() << endl;

	cout << endl;
}