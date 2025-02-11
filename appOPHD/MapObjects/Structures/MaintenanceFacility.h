#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"
#include "../../StorableResources.h"

#include "../../States/MapViewStateHelper.h" // yuck

#include <algorithm>


class MaintenanceFacility : public Structure
{
public:
	static constexpr int MaintenanceSuppliesCapacity{100};
	static constexpr int MaximumPersonnel{10};
	static constexpr int MinimumPersonnel{1};

public:
	MaintenanceFacility();

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

	int mMaterialsLevel{0};
	int mMaintenancePersonnel{MinimumPersonnel};
	int mAssignedPersonnel{0};

	StructureList mPriorityList;

	const StorableResources* mResources{nullptr};
};
