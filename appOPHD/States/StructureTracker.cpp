#include "StructureTracker.h"

#include <libOPHD/EnumStructureID.h>


namespace
{
	const std::vector<StructureID> DefaultAvailableSurfaceStructures = {
		StructureID::Agridome,
		StructureID::Chap,
		StructureID::CommTower,
		StructureID::HotLaboratory,
		StructureID::MaintenanceFacility,
		StructureID::Recycling,
		StructureID::Road,
		StructureID::RobotCommand,
		StructureID::Smelter,
		StructureID::SolarPanel1,
		StructureID::StorageTanks,
		StructureID::SurfaceFactory,
		StructureID::SurfacePolice,
		StructureID::Warehouse,
	};

	const std::vector<StructureID> DefaultAvailableUndergroundStructures = {
		StructureID::Laboratory,
		StructureID::Park,
		StructureID::UndergroundPolice,
		StructureID::RecreationCenter,
		StructureID::Residence,
		StructureID::UndergroundFactory,
		StructureID::MedicalCenter,
		StructureID::Nursery,
		StructureID::Commercial,
		StructureID::RedLightDistrict,
		StructureID::University,
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
