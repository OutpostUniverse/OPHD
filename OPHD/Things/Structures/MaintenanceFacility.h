#pragma once

#include "Structure.h"

#include <algorithm>

#include "../../Constants.h"
#include "../../StorableResources.h"

#include "../../States/MapViewStateHelper.h" // yuck


class MaintenanceFacility : public Structure
{
public:
	static constexpr int MaintenanceSuppliesCapacity{ 100 };
	static constexpr int MaximumPersonnel{ 10 };
	static constexpr int MinimumPersonnel{ 1 };

public:
	MaintenanceFacility() : Structure(constants::MaintenanceFacility,
		"structures/maintenance.sprite",
		StructureClass::Maintenance,
		StructureID::SID_MAINTENANCE_FACILITY)
	{
		maxAge(200);
		turnsToBuild(5);

		requiresCHAP(true);
		hasCrime(true);
	}

	void resources(const StorableResources& resources) { mResources = &resources; }

	int maintenancePersonnel() const { return mMaintenancePersonnel; }
	
	void addPersonnel()
	{
		mMaintenancePersonnel = std::clamp(mMaintenancePersonnel + 1, MinimumPersonnel, MaximumPersonnel);
	}
	
	void removePersonnel()
	{
		mMaintenancePersonnel = std::clamp(mMaintenancePersonnel - 1, MinimumPersonnel, MaximumPersonnel);
	}

	int personnel() const { return mMaintenancePersonnel; }

protected:
	void defineResourceInput() override
	{
		energyRequired(4);
	}

	void think() override
	{
		if (mMaterialsLevel == MaintenanceSuppliesCapacity) { return; }

		StorableResources maintenanceSuppliesCost{ 1, 1, 1, 1 };

		if (resources() >= maintenanceSuppliesCost)
		{
			removeRefinedResources(maintenanceSuppliesCost);
			mMaterialsLevel = std::clamp(mMaterialsLevel + 1, 0, MaintenanceSuppliesCapacity);
		}
	}


private:
	const StorableResources& resources() { return *mResources; }

	int mMaterialsLevel{ 0 };
	int mMaintenancePersonnel{ MinimumPersonnel };

	const StorableResources* mResources{ nullptr };
};
