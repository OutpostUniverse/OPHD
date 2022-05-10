#include "StructureTracker.h"

#include "../Constants/Strings.h"

namespace
{
	void addItemToList(const StructureTracker::StructureItem& structureItem, StructureTracker::StructureItemList& list)
	{
		for (const auto& item : list)
		{
			if (item.id == structureItem.id)
			{
				return;
			}
		}

		list.push_back(structureItem);
	}
};


StructureTracker::StructureTracker()
{
	mDefaultSurfaceStructures.push_back({constants::Agridome, 5, StructureID::SID_AGRIDOME});
	mDefaultSurfaceStructures.push_back({constants::Chap, 3, StructureID::SID_CHAP});
	mDefaultSurfaceStructures.push_back({constants::CommTower, 22, StructureID::SID_COMM_TOWER});
	mDefaultSurfaceStructures.push_back({constants::HotLaboratory, 18, StructureID::SID_HOT_LABORATORY});
	mDefaultSurfaceStructures.push_back({constants::MaintenanceFacility, 54, StructureID::SID_MAINTENANCE_FACILITY});
	mDefaultSurfaceStructures.push_back({constants::Recycling, 16, StructureID::SID_RECYCLING});
	mDefaultSurfaceStructures.push_back({constants::Road, 24, StructureID::SID_ROAD});
	mDefaultSurfaceStructures.push_back({constants::RobotCommand, 14, StructureID::SID_ROBOT_COMMAND});
	mDefaultSurfaceStructures.push_back({constants::Smelter, 4, StructureID::SID_SMELTER});
	mDefaultSurfaceStructures.push_back({constants::SolarPanel1, 33, StructureID::SID_SOLAR_PANEL1});
	mDefaultSurfaceStructures.push_back({constants::StorageTanks, 8, StructureID::SID_STORAGE_TANKS});
	mDefaultSurfaceStructures.push_back({constants::SurfaceFactory, 11, StructureID::SID_SURFACE_FACTORY});
	mDefaultSurfaceStructures.push_back({constants::SurfacePolice, 23, StructureID::SID_SURFACE_POLICE});
	mDefaultSurfaceStructures.push_back({constants::Warehouse, 9, StructureID::SID_WAREHOUSE});

	mDefaultUndergroundStructures.push_back({constants::Laboratory, 58, StructureID::SID_LABORATORY});
	mDefaultUndergroundStructures.push_back({constants::Park, 75, StructureID::SID_PARK});
	mDefaultUndergroundStructures.push_back({constants::UndergroundPolice, 61, StructureID::SID_UNDERGROUND_POLICE});
	mDefaultUndergroundStructures.push_back({constants::RecreationCenter, 73, StructureID::SID_RECREATION_CENTER});
	mDefaultUndergroundStructures.push_back({constants::Residence, 55, StructureID::SID_RESIDENCE});
	mDefaultUndergroundStructures.push_back({constants::UndergroundFactory, 69, StructureID::SID_UNDERGROUND_FACTORY});
	mDefaultUndergroundStructures.push_back({constants::MedicalCenter, 62, StructureID::SID_MEDICAL_CENTER});
	mDefaultUndergroundStructures.push_back({constants::Nursery, 77, StructureID::SID_NURSERY});
	mDefaultUndergroundStructures.push_back({constants::Commercial, 66, StructureID::SID_COMMERCIAL});
	mDefaultUndergroundStructures.push_back({constants::RedLightDistrict, 76, StructureID::SID_RED_LIGHT_DISTRICT});
	mDefaultUndergroundStructures.push_back({constants::University, 63, StructureID::SID_UNIVERSITY});
}


void StructureTracker::addUnlockedSurfaceStructure(const StructureItem& structureItem)
{
	addItemToList(structureItem, mUnlockedSurfaceStructures);
}


void StructureTracker::addUnlockedUndergroundStructure(const StructureItem& structureItem)
{
	addItemToList(structureItem, mUnlockedUndergroundStructures);
}
