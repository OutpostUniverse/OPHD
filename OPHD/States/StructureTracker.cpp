#include "StructureTracker.h"

#include "../Constants/Strings.h"

namespace
{
	void addItemToList(const IconGrid::Item& structureItem, StructureTracker::StructureItemList& list)
	{
		for (const auto& item : list)
		{
			if (item.meta == structureItem.meta)
			{
				return;
			}
		}

		list.push_back(structureItem);
	}
}


StructureTracker::StructureTracker()
{
	mAvailableSurfaceStructures.push_back({constants::Agridome, 5, StructureID::SID_AGRIDOME});
	mAvailableSurfaceStructures.push_back({constants::Chap, 3, StructureID::SID_CHAP});
	mAvailableSurfaceStructures.push_back({constants::CommTower, 22, StructureID::SID_COMM_TOWER});
	mAvailableSurfaceStructures.push_back({constants::HotLaboratory, 18, StructureID::SID_HOT_LABORATORY});
	mAvailableSurfaceStructures.push_back({constants::MaintenanceFacility, 54, StructureID::SID_MAINTENANCE_FACILITY});
	mAvailableSurfaceStructures.push_back({constants::Recycling, 16, StructureID::SID_RECYCLING});
	mAvailableSurfaceStructures.push_back({constants::Road, 24, StructureID::SID_ROAD});
	mAvailableSurfaceStructures.push_back({constants::RobotCommand, 14, StructureID::SID_ROBOT_COMMAND});
	mAvailableSurfaceStructures.push_back({constants::Smelter, 4, StructureID::SID_SMELTER});
	mAvailableSurfaceStructures.push_back({constants::SolarPanel1, 33, StructureID::SID_SOLAR_PANEL1});
	mAvailableSurfaceStructures.push_back({constants::StorageTanks, 8, StructureID::SID_STORAGE_TANKS});
	mAvailableSurfaceStructures.push_back({constants::SurfaceFactory, 11, StructureID::SID_SURFACE_FACTORY});
	mAvailableSurfaceStructures.push_back({constants::SurfacePolice, 23, StructureID::SID_SURFACE_POLICE});
	mAvailableSurfaceStructures.push_back({constants::Warehouse, 9, StructureID::SID_WAREHOUSE});

	mAvailableUndergroundStructures.push_back({constants::Laboratory, 58, StructureID::SID_LABORATORY});
	mAvailableUndergroundStructures.push_back({constants::Park, 75, StructureID::SID_PARK});
	mAvailableUndergroundStructures.push_back({constants::UndergroundPolice, 61, StructureID::SID_UNDERGROUND_POLICE});
	mAvailableUndergroundStructures.push_back({constants::RecreationCenter, 73, StructureID::SID_RECREATION_CENTER});
	mAvailableUndergroundStructures.push_back({constants::Residence, 55, StructureID::SID_RESIDENCE});
	mAvailableUndergroundStructures.push_back({constants::UndergroundFactory, 69, StructureID::SID_UNDERGROUND_FACTORY});
	mAvailableUndergroundStructures.push_back({constants::MedicalCenter, 62, StructureID::SID_MEDICAL_CENTER});
	mAvailableUndergroundStructures.push_back({constants::Nursery, 77, StructureID::SID_NURSERY});
	mAvailableUndergroundStructures.push_back({constants::Commercial, 66, StructureID::SID_COMMERCIAL});
	mAvailableUndergroundStructures.push_back({constants::RedLightDistrict, 76, StructureID::SID_RED_LIGHT_DISTRICT});
	mAvailableUndergroundStructures.push_back({constants::University, 63, StructureID::SID_UNIVERSITY});
}


void StructureTracker::addUnlockedSurfaceStructure(const IconGrid::Item& structureItem)
{
	addItemToList(structureItem, mAvailableSurfaceStructures);
}


void StructureTracker::addUnlockedUndergroundStructure(const IconGrid::Item& structureItem)
{
	addItemToList(structureItem, mAvailableUndergroundStructures);
}
