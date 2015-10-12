#include "StructureManager.h"

#include "Strings.h"

StructureManager::StructureManager() :	mDeferInsert(false),
										mChapActive(false)
{}


StructureManager::~StructureManager()
{}


void StructureManager::update()
{
	updateStructures();
}


void StructureManager::processResources(Resources& _r)
{
	// Process output resources first as all structures from the previous turn are still operational.
	processResourcesOut(_r);
	processResourcesIn(_r);
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
void StructureManager::processResourcesIn(Resources& _r)
{
	auto struct_it = mStructureList.begin();

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
				struct_it->first->enabled(false);
				struct_it->first->sprite().color(255, 0, 0, 185);
			}
		}
		++struct_it;
	}
}


void StructureManager::processResourcesOut(Resources& _r)
{
	// RESOURCES OUT
	Resources out;
	auto struct_it = mStructureList.begin();
	while (struct_it != mStructureList.end())
	{
		if (!struct_it->first->underConstruction() && !struct_it->first->idle() && struct_it->first->enabled())
			out += struct_it->first->resourcesOut();

		++struct_it;
	}

	// Adjust resources
	_r += out;
	_r.energy = out.energy; // energy is not cumulative
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

	/// We're in the process of updating structures so defer adding until the updates are finished.
	if(mDeferInsert)
	{
		mDeferredList[st] = TilePositionInfo(t, x, y, depth);
		return false;
	}

	if(mStructureList.find(st) != mStructureList.end())
	{
		cout << "StructureManager::addStructre(): Attempting to add a duplicate Structure* pointer." << endl;
		return false;
	}

	if(!clear)
		t->removeThing();

	mStructureList[st] = TilePositionInfo(t, x, y, depth);
	t->pushThing(st);
	t->thingIsStructure(true);

	return true;
}


/**
 * Removes a Structure from the StructureManager.
 *
 * \return	True if removed successfully. False if the structure is not found.
 */
bool StructureManager::removeStructure(Structure* st)
{
	if (mStructureList.erase(st) > 0)
		return true;

	return false;
}


void StructureManager::disconnectAll()
{
	for (auto st_it = mStructureList.begin(); st_it != mStructureList.end(); ++st_it)
		st_it->second.tile->connected(false);
}