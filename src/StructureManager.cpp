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
	{
		mStructureTileTable[*it]->deleteThing();
	}

	mStructureList.clear(); // Redundant?
	mStructureTileTable.clear(); // Redundant?
}


void StructureManager::update(ResourcePool& _r)
{
	disconnectAll();
	updateStructures();
	updateFactories();
}


void StructureManager::updateStructures()
{
	mChapActive = false;

	// Some structures can generate others (like the seed lander) so we need to make sure that we're
	// not inserting new structures while we're in the middle of iterating through the current list.
	setDeferredFlag(true);

	Structure* structure = nullptr;
	for (size_t i = 0; i < mStructureList.size(); ++i)
	{
		structure = mStructureList[i];

		structure->update();

		// FIXME:	Naive approach?
		if (structure->providesCHAP() && structure->operational())
			mChapActive = true;

		// FIXME: Are there any situations -at all- where a Structure's parent dead flag would be called?
		if (structure->dead())
			removeStructure(structure);
	}

	setDeferredFlag(false);
	copyDeferred();
}





void StructureManager::updateFactories()
{
	for (size_t i = 0; i < mFactoryList.size(); ++i)
	{
		mFactoryList[i]->updateProduction();
	}

}


void StructureManager::copyDeferred()
{
	// could just use std::vector.insert() but addStructure()
	// performs a number of checks that vector.insert() doesn't.
	auto it = mDeferredList.begin();
	while(it != mDeferredList.end())
	{
		addStructure((*it), mDeferredTileTable[(*it)], true);
		++it;
	}

	mDeferredList.clear();
	mDeferredTileTable.clear();
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

	// We're in the process of updating structures so defer adding until the updates are finished.
	if(mDeferInsert)
	{
		addToList(mDeferredList, mDeferredTileTable, st, t);
		return false;
	}

	for (size_t i = 0; i < mStructureList.size(); ++i)
	{
		if(mStructureList[i] == st)
			throw Exception(0, "Duplicate Structure!", "StructureManager::addStructure(): Attempting to add a Structure that's already managed.");
	}

	if(!clear)
		t->removeThing();

	addToList(mStructureList, mStructureTileTable, st, t);
	t->pushThing(st);
	t->thingIsStructure(true);

	if (st->isFactory())
	{
		mFactoryList.push_back(static_cast<Factory*>(st));
	}

	return true;
}


/**
 * Adds a Structure to a references list.
 * 
 * \param _list	Reference to a StructureList.
 * \param _map	Reference to a StructureMap.
 * \param _st	Pointer to a Structure. NOTE: Structure now becomes owned by StructureManager -- upon destruction memory allocated for the pointers is freed.
 * \param _t	Pointer to a Tile. NOTE: Pointer not owned by StructureManager -- memory managed by TileMap.
 * 
 * \note	Sorts the list by Structure Priority in ascending order. Higher priority structures (like the CC)
 *			will appear at the beginning of the list.
 */
void StructureManager::addToList(StructureList& _list, StructureMap& _map, Structure* _st, Tile* _t)
{
	_list.push_back(_st);
	sort(mStructureList.begin(), mStructureList.end(), [](const Structure* lhs, const Structure* rhs) { return lhs->priority() > rhs->priority(); });

	_map[_st] = _t;
}


/**
 * Removes a Structure from the StructureManager.
 * 
 * \warning	A Structure removed from the StructureManager will be freed.
 *			Remaining pointers and references will be invalidated.
 *
 * \return	True if removed successfully. False if the structure is not found.
 */
bool StructureManager::removeStructure(Structure* st)
{
	for (size_t i = 0; i < mStructureList.size(); ++i)
	{
		if (mStructureList[i] == st)
		{
			Structure* structure = mStructureList[i];

			auto it = mStructureTileTable.find(st);
			if (it != mStructureTileTable.end())
			{

				if (structure->isFactory())
					removeFactory(static_cast<Factory*>(structure));

				delete structure;
				structure = nullptr; // seems moot since entry is removed literally on the next line.
				mStructureList.erase(mStructureList.begin() + i);

				it->second->deleteThing();
				mStructureTileTable.erase(it);

				return true;
			}
			else
			{
				// If the structure is in the structure list but not in the structure to tile mapping table,
				// this should be considered a serious problem and we should barf immediately.
				throw Exception(0, "Rogue Structure!", "StructureManager::removeStructure(): Called with a pointer to a Structure that is not mapped to a Tile!");
				return false;
			}
		}
	}

	// If we hit this point we're calling the remove function on a structure that was never added to the manager which == BAD
	throw Exception(0, "Rogue Structure!", "StructureManager::removeStructure(): Called with a pointer to a Structure that is not managed!");
	return false;
}



void StructureManager::removeFactory(Factory* _f)
{
	for (size_t i = 0; i < mFactoryList.size(); ++i)
	{
		if (mFactoryList[i] == _f)
		{
			mFactoryList.erase(mFactoryList.begin() + i);
			return;
		}
	}
}


/**
 * Resets the 'connected' flag on all structures in the primary structure list.
 */
void StructureManager::disconnectAll()
{
	for (auto st_it = mStructureTileTable.begin(); st_it != mStructureTileTable.end(); ++st_it)
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
		cout << mStructureList[i]->name() << endl;
	cout << endl;
}