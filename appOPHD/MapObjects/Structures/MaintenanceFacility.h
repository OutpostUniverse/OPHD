#pragma once

#include "../Structure.h"

#include <vector>


struct StorableResources;

using StructureList = std::vector<Structure*>;


class MaintenanceFacility : public Structure
{
public:
	MaintenanceFacility(Tile& tile);

	void resources(const StorableResources& resources);
	bool suppliesAvailable() const;

	int maintenancePersonnel() const;
	void addPersonnel();
	void removePersonnel();
	int personnel() const;
	void personnel(int assigned);
	bool personnelAvailable() const;

	void addPriorityStructure(Structure* structure);
	void removePriorityStructure(Structure* structure);
	bool hasPriorityStructures() const;

	void repairStructures(StructureList& structures);

protected:
	bool canMakeRepairs() const;

	void moveStructureToBack(StructureList& structures, Structure* structure);

	void repairPriorityStructures(StructureList& structures);
	void repairStructure(Structure* structure);

	void think() override;

private:
	const StorableResources& resources();

	int mMaterialsLevel;
	int mMaintenancePersonnel;
	int mAssignedPersonnel;

	StructureList mPriorityList;

	const StorableResources* mResources;
};
