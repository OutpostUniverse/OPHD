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


bool StructureManager::CHAPAvailable() const
{
	for (size_t i = 0; i < mCHAPList.size(); ++i)
		if (mCHAPList[i]->operational())
			return true;

	return false;
}


void StructureManager::update(ResourcePool& _r)
{
	//disconnectAll();

	updateStructures(_r);
	updateFactories();
}


void StructureManager::updateStructures(ResourcePool& _r)
{
	// Captured here to avoid multiple list searches and to capture CHAP availability at the
	// beginning of the turn, something that could change during structure update loop.
	mChapActive = CHAPAvailable();


	setDeferredFlag(true);
	Structure* structure = nullptr;
	for (size_t i = 0; i < mStructureList.size(); ++i)
	{
		structure = mStructureList[i];
		structure->update();

		// State Check
		// ASSUMPTION:	Construction sites are considered self sufficient until they are
		//				completed and connected to the rest of the colony.
		if (structure->underConstruction())
			continue; // FIXME: smells of bad code, consider a different control path.

		// Connection Check
		if (!structure->selfSustained() && !mStructureTileTable[structure]->connected())
		{
			structure->disable();
			continue; // FIXME: smells of bad code, consider a different control path.
		}


		// CHAP Check
		if (structure->requiresCHAP() && !mChapActive)
			structure->disable();

		// handle input resources
		if (structure->resourcesIn().empty() || structure->enoughResourcesAvailable(_r))
			structure->enable();
		else
			structure->disable();


		if(structure->operational())
			structure->think();

		// handle output resources

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


/**
 * Sets the deffered insertion flag.
 * 
 * Some structures can generated others during their update function (such as the SEED Lander) so
 * this flag is used to defer structure insertion until after the update loop has completed.
 */
void StructureManager::setDeferredFlag(bool _b)
{
	mDeferInsert = _b;
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
 * \param	Call tile method that frees memory or not.
 */
void StructureManager::addStructure(Structure* st, Tile* t, bool clear)
{
	if(!t)
		return;

	// We're in the process of updating structures so defer adding until the updates are finished.
	if(mDeferInsert)
	{
		addToList(mDeferredList, mDeferredTileTable, st, t);
		return;
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

	if (st->providesCHAP())
		mCHAPList.push_back(st);
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
void StructureManager::removeStructure(Structure* st)
{
	for (size_t i = 0; i < mStructureList.size(); ++i)
	{
		if (mStructureList[i] == st)
		{
			Structure* structure = mStructureList[i];

			auto it = mStructureTileTable.find(st);
			if (it != mStructureTileTable.end())
			{
				
				// Maintain Specialzied Lists
				if (structure->isFactory())
					removeFactory(static_cast<Factory*>(structure));

				if (structure->providesCHAP())
					removeStructure(mCHAPList, structure);

				removeStructure(mStructureList, structure);
				
				it->second->deleteThing();
				mStructureTileTable.erase(it);

				return;
			}
			else
			{
				// If the structure is in the structure list but not in the structure to tile mapping table,
				// this should be considered a serious problem and we should barf immediately.
				throw Exception(0, "Rogue Structure!", "StructureManager::removeStructure(): Called with a pointer to a Structure that is not mapped to a Tile!");
			}
		}
	}

	// If we hit this point we're calling the remove function on a structure that was never added to the manager which == BAD
	throw Exception(0, "Rogue Structure!", "StructureManager::removeStructure(): Called with a pointer to a Structure that is not managed!");
}



/**
 * Removes a Factory from the Factory List.
 * 
 * \note	Does not free memory associated with the Factory.
 * 
 * \note	This function purely because the Factory list stores pointers to
 *			Factory objects, not Structure objects.
 */
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
* Removes a Structure from a given StructureList.
*
* \note	Does not free memory associated with the Structure.
*/
void StructureManager::removeStructure(StructureList& _sl, Structure* _s)
{
	for (size_t i = 0; i < _sl.size(); ++i)
	{
		if (_sl[i] == _s)
		{
			_sl.erase(_sl.begin() + i);
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