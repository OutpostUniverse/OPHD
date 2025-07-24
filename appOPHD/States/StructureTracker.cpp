#include "StructureTracker.h"


namespace
{
	const std::vector<StructureID> DefaultAvailableSurfaceStructures = {
		StructureID::SID_AGRIDOME,
		StructureID::SID_CHAP,
		StructureID::SID_COMM_TOWER,
		StructureID::SID_HOT_LABORATORY,
		StructureID::SID_MAINTENANCE_FACILITY,
		StructureID::SID_RECYCLING,
		StructureID::SID_ROAD,
		StructureID::SID_ROBOT_COMMAND,
		StructureID::SID_SMELTER,
		StructureID::SID_SOLAR_PANEL1,
		StructureID::SID_STORAGE_TANKS,
		StructureID::SID_SURFACE_FACTORY,
		StructureID::SID_SURFACE_POLICE,
		StructureID::SID_WAREHOUSE,
	};

	const std::vector<StructureID> DefaultAvailableUndergroundStructures = {
		StructureID::SID_LABORATORY,
		StructureID::SID_PARK,
		StructureID::SID_UNDERGROUND_POLICE,
		StructureID::SID_RECREATION_CENTER,
		StructureID::SID_RESIDENCE,
		StructureID::SID_UNDERGROUND_FACTORY,
		StructureID::SID_MEDICAL_CENTER,
		StructureID::SID_NURSERY,
		StructureID::SID_COMMERCIAL,
		StructureID::SID_RED_LIGHT_DISTRICT,
		StructureID::SID_UNIVERSITY,
	};


	void addItemToList(StructureID structureId, std::vector<StructureID>& list)
	{
		for (const auto& item : list)
		{
			if (item == structureId)
			{
				return;
			}
		}

		list.push_back(structureId);
	}
}


StructureTracker::StructureTracker() :
	mAvailableSurfaceStructures{DefaultAvailableSurfaceStructures},
	mAvailableUndergroundStructures{DefaultAvailableUndergroundStructures}
{
}


const std::vector<StructureID>& StructureTracker::availableSurfaceStructures() const
{
	return mAvailableSurfaceStructures;
}


const std::vector<StructureID>& StructureTracker::availableUndergroundStructures() const
{
	return mAvailableUndergroundStructures;
}


void StructureTracker::addUnlockedSurfaceStructure(StructureID structureId)
{
	addItemToList(structureId, mAvailableSurfaceStructures);
}


void StructureTracker::addUnlockedUndergroundStructure(StructureID structureId)
{
	addItemToList(structureId, mAvailableUndergroundStructures);
}
