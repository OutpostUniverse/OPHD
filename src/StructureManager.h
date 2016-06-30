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

	void addStructure(Structure* st, Tile* t);
	void removeStructure(Structure* st);

	StructureList& structureList(Structure::StructureType _st) { return mStructureLists[_st]; }

	void disconnectAll();

	bool CHAPAvailable();
	int count() const;

	void update(ResourcePool& _r);

	int totalEnergyProduction() const { return mTotalEnergyOutput; }

	void serialize(TiXmlElement* _ti);

protected:

private:
	typedef map<Structure*, Tile*> StructureTileTable;
	typedef map<Structure::StructureType, StructureList> StructureTypeTable;


private:
	void updateStructures(ResourcePool& _r, StructureList& _sl);
	void updateFactoryProduction();

	bool structureConnected(Structure* st) { return mStructureTileTable[st]->connected(); }

	void updateEnergyProduction(ResourcePool& _r);

private:
	StructureTileTable	mStructureTileTable;		/**< List mapping Structure's to a particular tile. */
	StructureTypeTable	mStructureLists;			/**< Map containing all of the structure list types available. */

	int					mTotalEnergyOutput;			/**< Total energy output of all energy producers in the structure list. */
};
