#ifndef __STRUCTURE_MANAGER__
#define __STRUCTURE_MANAGER__

#include "Things/Structures/Structure.h"
#include "Resources.h"
#include "TilePositionInfo.h"

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
	void processResources(Resources& _r);

	bool addStructure(Structure* st, Tile* t, int x, int y, int depth, bool clear);
	bool removeStructure(Structure* st);

	void disconnectAll();

protected:

	void copyDeferred();
	void setDeferredFlag(bool _b) { mDeferInsert = _b; }

private:
	typedef map<Structure*, TilePositionInfo> StructureMap;
	typedef vector<Structure*> StructureList;

	void updateStructures();
	void processResourcesIn(Resources& _r);
	void processResourcesOut(Resources& _r);

	StructureMap		mStructureList;				/**< List of all structures with positional information. */
	StructureMap		mDeferredList;				/**< List of deferred insertions. */

	StructureList		mStructuresBuilding;		/**< Structures still under construction. */
	StructureList		mStructuresOperational;		/**< Structures that are operational. */
	StructureList		mStructuresIdle;			/**< Structures that are Idle. */
	StructureList		mStructuresDisabled;		/**< Structures that are disabled. */

	bool				mDeferInsert;				/**< Flag indicating that we're accessing the structure list and insertions should be deferred. */
	bool				mChapActive;				/**< Flag indicating that there is a functioning CHAP Facility. */
};


#endif