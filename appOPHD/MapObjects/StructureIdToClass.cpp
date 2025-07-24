#include "StructureIdToClass.h"

#include "StructureClass.h"

#include <libOPHD/EnumStructureID.h>

#include <array>
#include <string>
#include <stdexcept>


namespace
{
	constexpr auto structureIdToClassTable = std::array{
		StructureClass::Undefined, // SID_NONE
		StructureClass::FoodProduction, // SID_AGRIDOME
		StructureClass::Tube, // SID_AIR_SHAFT
		StructureClass::Lander, // SID_CARGO_LANDER
		StructureClass::LifeSupport, // SID_CHAP
		StructureClass::Lander, // SID_COLONIST_LANDER
		StructureClass::Command, // SID_COMMAND_CENTER
		StructureClass::Commercial, // SID_COMMERCIAL
		StructureClass::Communication, // SID_COMM_TOWER
		StructureClass::EnergyProduction, // SID_FUSION_REACTOR
		StructureClass::Laboratory, // SID_HOT_LABORATORY
		StructureClass::Laboratory, // SID_LABORATORY
		StructureClass::MedicalCenter, // SID_MEDICAL_CENTER
		StructureClass::Mine, // SID_MINE_FACILITY
		StructureClass::Undefined, // SID_MINE_SHAFT
		StructureClass::Nursery, // SID_NURSERY
		StructureClass::Park, // SID_PARK
		StructureClass::RecreationCenter, // SID_RECREATION_CENTER
		StructureClass::Residence, // SID_RED_LIGHT_DISTRICT
		StructureClass::Residence, // SID_RESIDENCE
		StructureClass::Road, // SID_ROAD
		StructureClass::RobotCommand, // SID_ROBOT_COMMAND
		StructureClass::Factory, // SID_SEED_FACTORY
		StructureClass::Lander, // SID_SEED_LANDER
		StructureClass::EnergyProduction, // SID_SEED_POWER
		StructureClass::Smelter, // SID_SEED_SMELTER
		StructureClass::Smelter, // SID_SMELTER
		StructureClass::EnergyProduction, // SID_SOLAR_PANEL1
		StructureClass::EnergyProduction, // SID_SOLAR_PLANT
		StructureClass::Storage, // SID_STORAGE_TANKS
		StructureClass::Factory, // SID_SURFACE_FACTORY
		StructureClass::SurfacePolice, // SID_SURFACE_POLICE
		StructureClass::Tube, // SID_TUBE
		StructureClass::Factory, // SID_UNDERGROUND_FACTORY
		StructureClass::UndergroundPolice, // SID_UNDERGROUND_POLICE
		StructureClass::University, // SID_UNIVERSITY
		StructureClass::Warehouse, // SID_WAREHOUSE
		StructureClass::Recycling, // SID_RECYCLING
		StructureClass::Maintenance, // SID_MAINTENANCE_FACILITY
	};


	static_assert(structureIdToClassTable.size() == static_cast<std::size_t>(StructureID::SID_COUNT));
}


StructureClass structureIdToClass(StructureID id) {
	const auto structureTypeIndex = static_cast<std::size_t>(id);
	if (structureTypeIndex >= structureIdToClassTable.size())
	{
		throw std::runtime_error("Unknown StructureID type: " + std::to_string(structureTypeIndex));
	}
	return structureIdToClassTable.at(structureTypeIndex);
}
