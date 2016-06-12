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

	void update(ResourcePool& _r);

	bool addStructure(Structure* st, Tile* t, bool clear);
	bool removeStructure(Structure* st);

	void disconnectAll();

	void printSortedList();

protected:

	void copyDeferred();
	void setDeferredFlag(bool _b) { mDeferInsert = _b; }

private:
	typedef map<Structure*, Tile*> StructureMap;
	typedef vector<Structure*> StructureList;
	typedef vector<Factory*> FactoryList;

	void updateStructures();
	void updateFactories();

	void addToList(StructureList& _list, StructureMap& _map, Structure* _st, Tile* _t);

	void removeFactory(Factory* _f);


	StructureList		mStructureList;				/**< List of all structures. */
	StructureList		mDeferredList;				/**< List of deferred insertions. */

	FactoryList			mFactoryList;				/**< List of factories. */

	StructureMap		mStructureTileTable;		/**< List mapping Structure's to a particular tile. */
	StructureMap		mDeferredTileTable;			/**< Deferred list mapping Structure's to a particular tile. */

	bool				mDeferInsert;				/**< Flag indicating that we're accessing the structure list and insertions should be deferred. */
	bool				mChapActive;				/**< Flag indicating that there is a functioning CHAP Facility. */
};
