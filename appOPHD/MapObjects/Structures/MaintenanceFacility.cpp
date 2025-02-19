#include "MaintenanceFacility.h"


#include "../../StorableResources.h"

#include "../../States/MapViewStateHelper.h"

#include <algorithm>


namespace
{
	constexpr int MaintenanceSuppliesCapacity{100};
	constexpr int MaximumPersonnel{10};
	constexpr int MinimumPersonnel{1};
}


MaintenanceFacility::MaintenanceFacility() :
	Structure(
		StructureClass::Maintenance,
		StructureID::SID_MAINTENANCE_FACILITY
	),
	mMaterialsLevel{0},
	mMaintenancePersonnel{MinimumPersonnel},
	mAssignedPersonnel{0},
	mResources{nullptr}
{
}


void MaintenanceFacility::resources(const StorableResources& resources)
{
	mResources = &resources;
}


bool MaintenanceFacility::suppliesAvailable() const
{
	return mMaterialsLevel > 0;
}


int MaintenanceFacility::maintenancePersonnel() const
{
	return mMaintenancePersonnel;
}


void MaintenanceFacility::addPersonnel()
{
	mMaintenancePersonnel = std::clamp(mMaintenancePersonnel + 1, MinimumPersonnel, MaximumPersonnel);
}


void MaintenanceFacility::removePersonnel()
{
	mMaintenancePersonnel = std::clamp(mMaintenancePersonnel - 1, MinimumPersonnel, MaximumPersonnel);
}


int MaintenanceFacility::personnel() const
{
	return mMaintenancePersonnel;
}


void MaintenanceFacility::personnel(int assigned)
{
	mMaintenancePersonnel = assigned;
}


bool MaintenanceFacility::personnelAvailable() const
{
	return mAssignedPersonnel < mMaintenancePersonnel;
}


void MaintenanceFacility::addPriorityStructure(Structure* structure)
{
	mPriorityList.push_back(structure);
}


void MaintenanceFacility::removePriorityStructure(Structure* structure)
{
	auto it = std::find(mPriorityList.begin(), mPriorityList.end(), structure);
	if (it != mPriorityList.end())
	{
		mPriorityList.erase(it);
	}
}


bool MaintenanceFacility::hasPriorityStructures() const
{
	return !mPriorityList.empty();
}


void MaintenanceFacility::repairStructures(StructureList& structures)
{
	if (!operational()) { return; }

	if (hasPriorityStructures())
	{
		repairPriorityStructures(structures);
	}

	for (auto* structure : structures)
	{
		repairStructure(structure);
		std::rotate(structures.begin(), structures.begin() + 1, structures.end());
	}
}


bool MaintenanceFacility::canMakeRepairs() const
{
	return (personnelAvailable() && suppliesAvailable());
}


void MaintenanceFacility::moveStructureToBack(StructureList& structures, Structure* structure)
{
	auto it = std::find(structures.begin(), structures.end(), structure);
	if (it != structures.end())
	{
		structures.erase(it);
		structures.push_back(structure);
	}
}


void MaintenanceFacility::repairPriorityStructures(StructureList& structures)
{
	for (auto* structure : mPriorityList)
	{
		if (!canMakeRepairs()) { return; }

		repairStructure(structure);

		moveStructureToBack(structures, structure);

		if (structure->operational())
		{
			removePriorityStructure(structure);
		}
	}
}


void MaintenanceFacility::repairStructure(Structure* structure)
{
	if (structure->destroyed() || structure->underConstruction()) { return; }

	if (!canMakeRepairs()) { return; }

	if (structure->disabled() && structure->disabledReason() == DisabledReason::StructuralIntegrity)
	{
		--mMaterialsLevel;
		++mAssignedPersonnel;
		if (structure->integrity() > 35) // \fixme magic number
		{
			structure->integrity(100);
			structure->enable();
		}
		else
		{
			structure->integrity(50);
			addPriorityStructure(structure);
		}
	}
	else if (structure->operational() || structure->isIdle())
	{
		--mMaterialsLevel;
		++mAssignedPersonnel;
		structure->integrity(100);

		if (structure->structureId() == StructureID::SID_ROAD)
		{
			structure->rebuild();
		}
	}
}


void MaintenanceFacility::think()
{
	if (mMaterialsLevel == MaintenanceSuppliesCapacity) { return; }

	StorableResources maintenanceSuppliesCost{1, 1, 1, 1};

	if (resources() >= maintenanceSuppliesCost)
	{
		removeRefinedResources(maintenanceSuppliesCost);
		mMaterialsLevel = std::clamp(mMaterialsLevel + 1, 0, MaintenanceSuppliesCapacity);
	}

	mAssignedPersonnel = 0;
}


const StorableResources& MaintenanceFacility::resources()
{
	return *mResources;
}
