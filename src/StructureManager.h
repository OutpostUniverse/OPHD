#pragma once

#include "Things/Structures/Structure.h"
#include "Things/Structures/Factory.h"

#include "ResourcePool.h"
#include "Map/Tile.h"

/**
 * Handles structure updating and resource management for structures.
 * 
 * Keeps track of which structures are operational, idle and disabled.
 */
class StructureManager
{
public:
	typedef std::vector<Structure*> StructureList;

public:
	StructureManager();
	~StructureManager();

	void addStructure(Structure* st, Tile* t);
	void removeStructure(Structure* st);

	StructureList& structureList(Structure::StructureClass _st) { return mStructureLists[_st]; }

	void disconnectAll();
	void dropAllStructures();

	int count() const;

	int getCountInState(Structure::StructureClass _st, Structure::StructureState _state);

	int disabled();
	int destroyed();

	bool CHAPAvailable();

	void updateEnergyProduction(ResourcePool& _r);
	int totalEnergyProduction() const { return mTotalEnergyOutput; }

	void update(ResourcePool& _r);

	void serialize(NAS2D::Xml::XmlElement* _ti);

protected:

private:
	typedef std::map<Structure*, Tile*> StructureTileTable;
	typedef std::map<Structure::StructureClass, StructureList> StructureClassTable;

private:
	void updateStructures(ResourcePool& _r, StructureList& _sl);
	void updateFactoryProduction();

	bool structureConnected(Structure* st) { return mStructureTileTable[st]->connected(); }

private:
	StructureTileTable	mStructureTileTable;		/**< List mapping Structure's to a particular tile. */
	StructureClassTable	mStructureLists;			/**< Map containing all of the structure list types available. */

	int					mTotalEnergyOutput;			/**< Total energy output of all energy producers in the structure list. */
};
