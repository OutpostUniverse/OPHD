#include "StructureTracker.h"

#include "../Constants/Strings.h"

namespace
{
	const std::vector<IconGrid::Item> DefaultAvailableSurfaceStructures = {
		{constants::Agridome, 5, StructureID::SID_AGRIDOME},
		{constants::Chap, 3, StructureID::SID_CHAP},
		{constants::CommTower, 22, StructureID::SID_COMM_TOWER},
		{constants::HotLaboratory, 18, StructureID::SID_HOT_LABORATORY},
		{constants::MaintenanceFacility, 54, StructureID::SID_MAINTENANCE_FACILITY},
		{constants::Recycling, 16, StructureID::SID_RECYCLING},
		{constants::Road, 24, StructureID::SID_ROAD},
		{constants::RobotCommand, 14, StructureID::SID_ROBOT_COMMAND},
		{constants::Smelter, 4, StructureID::SID_SMELTER},
		{constants::SolarPanel1, 33, StructureID::SID_SOLAR_PANEL1},
		{constants::StorageTanks, 8, StructureID::SID_STORAGE_TANKS},
		{constants::SurfaceFactory, 11, StructureID::SID_SURFACE_FACTORY},
		{constants::SurfacePolice, 23, StructureID::SID_SURFACE_POLICE},
		{constants::Warehouse, 9, StructureID::SID_WAREHOUSE},
	};

	const std::vector<IconGrid::Item> DefaultAvailableUndergroundStructures = {
		{constants::Laboratory, 58, StructureID::SID_LABORATORY},
		{constants::Park, 75, StructureID::SID_PARK},
		{constants::UndergroundPolice, 61, StructureID::SID_UNDERGROUND_POLICE},
		{constants::RecreationCenter, 73, StructureID::SID_RECREATION_CENTER},
		{constants::Residence, 55, StructureID::SID_RESIDENCE},
		{constants::UndergroundFactory, 69, StructureID::SID_UNDERGROUND_FACTORY},
		{constants::MedicalCenter, 62, StructureID::SID_MEDICAL_CENTER},
		{constants::Nursery, 77, StructureID::SID_NURSERY},
		{constants::Commercial, 66, StructureID::SID_COMMERCIAL},
		{constants::RedLightDistrict, 76, StructureID::SID_RED_LIGHT_DISTRICT},
		{constants::University, 63, StructureID::SID_UNIVERSITY},
	};

	void addItemToList(const IconGrid::Item& structureItem, std::vector<IconGrid::Item>& list)
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
	reset();
}


void StructureTracker::addUnlockedSurfaceStructure(const IconGrid::Item& structureItem)
{
	addItemToList(structureItem, mAvailableSurfaceStructures);
}


void StructureTracker::addUnlockedUndergroundStructure(const IconGrid::Item& structureItem)
{
	addItemToList(structureItem, mAvailableUndergroundStructures);
}


void StructureTracker::reset()
{
	mAvailableSurfaceStructures = DefaultAvailableSurfaceStructures;
	mAvailableUndergroundStructures = DefaultAvailableUndergroundStructures;
}
