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

	typedef vector<Structure*> StructureList;

public:

	StructureManager();

	~StructureManager();

	void update(ResourcePool& _r);

	bool addStructure(Structure* st, Tile* t, bool clear);
	bool removeStructure(Structure* st);

	bool CHAPAvailable() const;

	StructureList& CHAPFacilities() { return mCHAPList; }

	void disconnectAll();

	void printSortedList();

protected:

	void copyDeferred();
	void setDeferredFlag(bool _b);

private:

	typedef map<Structure*, Tile*> StructureMap;
	typedef vector<Factory*> FactoryList;

	void updateStructures(ResourcePool& _r);
	void updateFactories();

	void addToList(StructureList& _list, StructureMap& _map, Structure* _st, Tile* _t);

	void removeFactory(Factory* _f);
	void removeStructure(StructureList& _sl, Structure* _s);


	StructureList		mStructureList;				/**< List of all structures. */
	StructureList		mDeferredList;				/**< List of deferred insertions. */

	StructureList		mCHAPList;					/**< List of all CHAP Facilities. */

	FactoryList			mFactoryList;				/**< List of factories. */

	StructureMap		mStructureTileTable;		/**< List mapping Structure's to a particular tile. */
	StructureMap		mDeferredTileTable;			/**< Deferred list mapping Structure's to a particular tile. */

	bool				mDeferInsert;				/**< Flag indicating that we're accessing the structure list and insertions should be deferred. */
	bool				mChapActive;				/**< Flag indicating that there is a functioning CHAP Facility. */
};
