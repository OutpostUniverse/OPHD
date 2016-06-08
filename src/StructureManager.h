#pragma once

#include "Things/Structures/Structure.h"
#include "Things/Structures/Factory.h"

#include "ResourcePool.h"
#include "Tile.h"

/**
 * Handles structure updating and resource management for structures.
 * 
 * Keeps track of which structures are operational, idle and disabled.
 */
class StructureManager
{
public:
	StructureManager();

	~StructureManager();

	void update();
	void processResources(ResourcePool& _r);

	bool addStructure(Structure* st, Tile* t, bool clear);
	bool removeStructure(Structure* st);

	void disconnectAll();

	void printSortedList();

protected:

	void copyDeferred();
	void setDeferredFlag(bool _b) { mDeferInsert = _b; }

private:
	typedef std::pair<Structure*, Tile*> StructureTilePair;
	typedef vector<StructureTilePair> StructureMap;
	typedef vector<Structure*> StructureList;

	void updateStructures();
	void updateFactories();

	void processResourcesIn(ResourcePool& _r);
	void processResourcesOut(ResourcePool& _r);

	void addToList(StructureMap& _sm, Structure* _st, Tile* _t);
	bool removeStructureFromList(Structure* st, StructureList& list);

	StructureMap		mStructureList;				/**< List of all structures with positional information. */
	StructureMap		mDeferredList;				/**< List of deferred insertions. */

	StructureList		mStructuresBuilding;		/**< Structures still under construction. */
	StructureList		mStructuresOperational;		/**< Structures that are operational. */
	StructureList		mStructuresIdle;			/**< Structures that are Idle. */
	StructureList		mStructuresDisabled;		/**< Structures that are disabled. */

	StructureList		mFactoryList;				/**< List of factories. */

	bool				mDeferInsert;				/**< Flag indicating that we're accessing the structure list and insertions should be deferred. */
	bool				mChapActive;				/**< Flag indicating that there is a functioning CHAP Facility. */
};
