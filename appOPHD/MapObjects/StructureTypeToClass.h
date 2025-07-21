#pragma once

#include "StructureClass.h"

#include <type_traits>


class Agridome;
class AirShaft;
class CargoLander;
class ColonistLander;
class CommandCenter;
class CommTower;
class Factory;
class FoodProduction;
class HotLaboratory;
class Laboratory;
class MaintenanceFacility;
class MineFacility;
class MineShaft;
class OreRefining;
class PowerStructure;
class Recycling;
class ResearchFacility;
class Residence;
class Road;
class SeedFactory;
class SeedLander;
class StorageTanks;
class SurfaceFactory;
class Tube;
class UndergroundFactory;
class Warehouse;


template <typename T> constexpr bool dependent_false = false;


template <typename StructureType>
constexpr StructureClass structureTypeToClass() {
	if constexpr (std::is_same_v<StructureType, Agridome>) { return StructureClass::FoodProduction; }
	else if constexpr (std::is_same_v<StructureType, AirShaft>) { return StructureClass::Tube; }
	else if constexpr (std::is_same_v<StructureType, CargoLander>) { return StructureClass::Lander; }
	else if constexpr (std::is_same_v<StructureType, ColonistLander>) { return StructureClass::Lander; }
	else if constexpr (std::is_same_v<StructureType, CommandCenter>) { return StructureClass::Command; }
	else if constexpr (std::is_same_v<StructureType, CommTower>) { return StructureClass::Communication; }
	else if constexpr (std::is_same_v<StructureType, Factory>) { return StructureClass::Factory; }
	else if constexpr (std::is_same_v<StructureType, FoodProduction>) { return StructureClass::FoodProduction; }
	else if constexpr (std::is_same_v<StructureType, HotLaboratory>) { return StructureClass::Laboratory; }
	else if constexpr (std::is_same_v<StructureType, Laboratory>) { return StructureClass::Laboratory; }
	else if constexpr (std::is_same_v<StructureType, MaintenanceFacility>) { return StructureClass::Maintenance; }
	else if constexpr (std::is_same_v<StructureType, MineFacility>) { return StructureClass::Mine; }
	else if constexpr (std::is_same_v<StructureType, MineShaft>) { return StructureClass::Undefined; }
	else if constexpr (std::is_same_v<StructureType, OreRefining>) { return StructureClass::Smelter; }
	else if constexpr (std::is_same_v<StructureType, PowerStructure>) { return StructureClass::EnergyProduction; }
	else if constexpr (std::is_same_v<StructureType, Recycling>) { return StructureClass::Recycling; }
	else if constexpr (std::is_same_v<StructureType, ResearchFacility>) { return StructureClass::Laboratory; }
	else if constexpr (std::is_same_v<StructureType, Residence>) { return StructureClass::Residence; }
	else if constexpr (std::is_same_v<StructureType, Road>) { return StructureClass::Road; }
	else if constexpr (std::is_same_v<StructureType, SeedFactory>) { return StructureClass::Factory; }
	else if constexpr (std::is_same_v<StructureType, SeedLander>) { return StructureClass::Lander; }
	else if constexpr (std::is_same_v<StructureType, StorageTanks>) { return StructureClass::Storage; }
	else if constexpr (std::is_same_v<StructureType, SurfaceFactory>) { return StructureClass::Factory; }
	else if constexpr (std::is_same_v<StructureType, Tube>) { return StructureClass::Tube; }
	else if constexpr (std::is_same_v<StructureType, UndergroundFactory>) { return StructureClass::Factory; }
	else if constexpr (std::is_same_v<StructureType, Warehouse>) { return StructureClass::Warehouse; }
	else { static_assert(dependent_false<StructureType>, "Unknown type"); }
}
