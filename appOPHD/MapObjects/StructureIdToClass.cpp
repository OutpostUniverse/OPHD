#include "StructureIdToClass.h"

#include "StructureClass.h"

#include <libOPHD/EnumStructureID.h>

#include <array>
#include <string>
#include <stdexcept>


namespace
{
	constexpr auto structureIdToClassTable = std::array{
		StructureClass::Undefined, // None
		StructureClass::FoodProduction, // Agridome
		StructureClass::Tube, // AirShaft
		StructureClass::Lander, // CargoLander
		StructureClass::LifeSupport, // Chap
		StructureClass::Lander, // ColonistLander
		StructureClass::Command, // CommandCenter
		StructureClass::Commercial, // Commercial
		StructureClass::Communication, // CommTower
		StructureClass::EnergyProduction, // FusionReactor
		StructureClass::Laboratory, // HotLaboratory
		StructureClass::Laboratory, // Laboratory
		StructureClass::MedicalCenter, // MedicalCenter
		StructureClass::Mine, // MineFacility
		StructureClass::Undefined, // MineShaft
		StructureClass::Nursery, // Nursery
		StructureClass::Park, // Park
		StructureClass::RecreationCenter, // RecreationCenter
		StructureClass::Residence, // RedLightDistrict
		StructureClass::Residence, // Residence
		StructureClass::Road, // Road
		StructureClass::RobotCommand, // RobotCommand
		StructureClass::Factory, // SeedFactory
		StructureClass::Lander, // SeedLander
		StructureClass::EnergyProduction, // SeedPower
		StructureClass::Smelter, // SeedSmelter
		StructureClass::Smelter, // Smelter
		StructureClass::EnergyProduction, // SolarPanel1
		StructureClass::EnergyProduction, // SolarPlant
		StructureClass::Storage, // StorageTanks
		StructureClass::Factory, // SurfaceFactory
		StructureClass::SurfacePolice, // SurfacePolice
		StructureClass::Tube, // Tube
		StructureClass::Factory, // UndergroundFactory
		StructureClass::UndergroundPolice, // UndergroundPolice
		StructureClass::University, // University
		StructureClass::Warehouse, // Warehouse
		StructureClass::Recycling, // Recycling
		StructureClass::Maintenance, // MaintenanceFacility
	};


	static_assert(structureIdToClassTable.size() == static_cast<std::size_t>(StructureID::Count));
}


StructureClass structureIdToClass(StructureID id) {
	const auto structureTypeIndex = static_cast<std::size_t>(id);
	if (structureTypeIndex >= structureIdToClassTable.size())
	{
		throw std::runtime_error("Unknown StructureID type: " + std::to_string(structureTypeIndex));
	}
	return structureIdToClassTable.at(structureTypeIndex);
}
