#include "StructureManager.h"

#include "Constants.h"

#include <algorithm>


StructureManager::StructureManager() :	mChapActive(false)
{}


/**
 * D'tor
 * 
 * \note	Things (aka Structures) are deleted whenever a Tile
 *			is deleted so this d'tor really doesn't need to do
 *			anything special.
 */
StructureManager::~StructureManager()
{}


bool StructureManager::CHAPAvailable()
{
	for (size_t i = 0; i < mStructureLists[Structure::STRUCTURE_ATMOSPHERE_PRODUCTION].size(); ++i)
		if (mStructureLists[Structure::STRUCTURE_ATMOSPHERE_PRODUCTION][i]->operational())
			return true;

	return false;
}


void StructureManager::update(ResourcePool& _r)
{
	// Captured here to avoid multiple list searches and to capture CHAP availability at the
	// beginning of the turn, something that could change during structure update loop.
	mChapActive = CHAPAvailable();

	updateStructures(_r, mStructureLists[Structure::STRUCTURE_LANDER]);
	updateStructures(_r, mStructureLists[Structure::STRUCTURE_COMMAND]);
	updateStructures(_r, mStructureLists[Structure::STRUCTURE_ENERGY_PRODUCTION]);
	updateStructures(_r, mStructureLists[Structure::STRUCTURE_ATMOSPHERE_PRODUCTION]);
	updateStructures(_r, mStructureLists[Structure::STRUCTURE_FACTORY]);

	/*
	int energyCount = 0;
	for (size_t i = 0; i < mEnergyProducers.size(); ++i)
	{
		if (mEnergyProducers[i]->operational())
			energyCount += mEnergyProducers[i]->resourcesOut().energy();
	}

	_r.energy(energyCount);
	*/

	updateFactoryProduction();
}


void StructureManager::updateStructures(ResourcePool& _r, StructureList& _sl)
{
	Structure* structure = nullptr;
	for (size_t i = 0; i < _sl.size(); ++i)
	{
		structure = _sl[i];
		structure->update();

		// State Check
		// ASSUMPTION:	Construction sites are considered self sufficient until they are
		//				completed and connected to the rest of the colony.
		if (structure->underConstruction())
			continue; // FIXME: smells of bad code, consider a different control path.

		// Connection Check
		if (!structure->selfSustained() && !structureConnected(structure))
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
}


void StructureManager::updateFactoryProduction()
{
	StructureList& sl = mStructureLists[Structure::STRUCTURE_FACTORY];

	for (size_t i = 0; i < sl.size(); ++i)
	{
		static_cast<Factory*>(sl[i])->updateProduction();
	}
}


/**
 * Adds a new Structure to the StructureManager.
 */
void StructureManager::addStructure(Structure* st, Tile* t)
{
	// Sanity checks
	if(!t)
		return;

	if (!t->empty())
	{
		throw Exception(0, "Tile Not Empty", "StructureManager::addStructure(): Attempting to add a structure to a Tile that is not empty.");
		return;
	}

	if (mStructureTileTable.find(st) != mStructureTileTable.end())
	{
		throw Exception(0, "Duplicate Structure", "StructureManager::addStructure(): Attempting to add a Structure that is already managed!");
		return;
	}

	mStructureTileTable[st] = t;
	
	mStructureLists[st->type()].push_back(st);
	t->pushThing(st);
	t->thingIsStructure(true);
}


/**
 * Removes a Structure from the StructureManager.
 * 
 * \warning	A Structure removed from the StructureManager will be freed.
 *			Remaining pointers and references will be invalidated.
 */
void StructureManager::removeStructure(Structure* st)
{
	StructureList& sl = mStructureLists[st->type()];

	if (sl.empty())
	{
		throw Exception(0, "Rogue Structure", "StructureManager::removeStructure(): Attempting to remove a Structure that is not managed by the StructureManager.");
		return; //overkill?
	}

	for (size_t i = 0; i < sl.size(); ++i)
	{
		if (sl[i] == st)
		{
			sl.erase(sl.begin() + i);
			break;
		}
	}

	auto tileTableIt = mStructureTileTable.find(st);
	if (tileTableIt == mStructureTileTable.end())
	{
		throw Exception(0, "Rogue Structure", "StructureManager::removeStructure(): Attempting to remove a Structure that is not managed by the StructureManager.");
		return; //overkill?
	}
	else
	{
		tileTableIt->second->deleteThing();
		mStructureTileTable.erase(tileTableIt);
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
* Returns the number of structures currently being managed by the StructureManager.
*/
int StructureManager::count() const
{
	int count = 0;
	for (auto it = mStructureLists.begin(); it != mStructureLists.end(); ++it)
		count += it->second.size();
	
	return count;
}