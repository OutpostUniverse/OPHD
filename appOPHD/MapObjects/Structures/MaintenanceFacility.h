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
	MaintenanceFacility() : Structure(
		StructureClass::Maintenance,
		StructureID::SID_MAINTENANCE_FACILITY)
	{
	}


	void resources(const StorableResources& resources)
	{
		mResources = &resources;
	}


	bool suppliesAvailable() const
	{
		return mMaterialsLevel > 0;
	}


	int maintenancePersonnel() const
	{
		return mMaintenancePersonnel;
	}


	void addPersonnel()
	{
		mMaintenancePersonnel = std::clamp(mMaintenancePersonnel + 1, MinimumPersonnel, MaximumPersonnel);
	}


	void removePersonnel()
	{
		mMaintenancePersonnel = std::clamp(mMaintenancePersonnel - 1, MinimumPersonnel, MaximumPersonnel);
	}


	int personnel() const
	{
		return mMaintenancePersonnel;
	}


	void personnel(int assigned)
	{
		mMaintenancePersonnel = assigned;
	}


	bool personnelAvailable() const
	{
		return mAssignedPersonnel < mMaintenancePersonnel;
	}


	void addPriorityStructure(Structure* structure)
	{
		mPriorityList.push_back(structure);
	}


	void removePriorityStructure(Structure* structure)
	{
		auto it = std::find(mPriorityList.begin(), mPriorityList.end(), structure);
		if (it != mPriorityList.end())
		{
			mPriorityList.erase(it);
		}
	}


	bool hasPriorityStructures() const
	{
		return !mPriorityList.empty();
	}


	void repairStructures(StructureList& structures)
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


protected:
	bool canMakeRepairs() const
	{
		return (personnelAvailable() && suppliesAvailable());
	}


	void moveStructureToBack(StructureList& structures, Structure* structure)
	{
		auto it = std::find(structures.begin(), structures.end(), structure);
		if (it != structures.end())
		{
			structures.erase(it);
			structures.push_back(structure);
		}
	}


	void repairPriorityStructures(StructureList& structures)
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


	void repairStructure(Structure* structure)
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


	void think() override
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


private:
	const StorableResources& resources() { return *mResources; }

	int mMaterialsLevel{0};
	int mMaintenancePersonnel{MinimumPersonnel};
	int mAssignedPersonnel{0};

	StructureList mPriorityList;

	const StorableResources* mResources{nullptr};
};
