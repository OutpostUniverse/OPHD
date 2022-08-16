#pragma once

#include "Things/Structures/Structure.h"
#include "Things/Structures/Structures.h"

#include <map>


namespace NAS2D
{
	namespace Xml
	{
		class XmlElement;
	}
}

class Tile;
class PopulationPool;
struct StorableResources;


template <typename T> constexpr bool dependent_false = false;

template <typename StructureType>
constexpr Structure::StructureClass structureTypeToClass() {
	if constexpr (std::is_same_v<StructureType, Agridome>) { return Structure::StructureClass::FoodProduction; }
	else if constexpr (std::is_same_v<StructureType, AirShaft>) { return Structure::StructureClass::Tube; }
	else if constexpr (std::is_same_v<StructureType, CargoLander>) { return Structure::StructureClass::Lander; }
	else if constexpr (std::is_same_v<StructureType, CHAP>) { return Structure::StructureClass::LifeSupport; }
	else if constexpr (std::is_same_v<StructureType, ColonistLander>) { return Structure::StructureClass::Lander; }
	else if constexpr (std::is_same_v<StructureType, CommandCenter>) { return Structure::StructureClass::Command; }
	else if constexpr (std::is_same_v<StructureType, Commercial>) { return Structure::StructureClass::Commercial; }
	else if constexpr (std::is_same_v<StructureType, CommTower>) { return Structure::StructureClass::Communication; }
	else if constexpr (std::is_same_v<StructureType, Factory>) { return Structure::StructureClass::Factory; }
	else if constexpr (std::is_same_v<StructureType, FoodProduction>) { return Structure::StructureClass::FoodProduction; }
	else if constexpr (std::is_same_v<StructureType, FusionReactor>) { return Structure::StructureClass::EnergyProduction; }
	else if constexpr (std::is_same_v<StructureType, HotLaboratory>) { return Structure::StructureClass::Laboratory; }
	else if constexpr (std::is_same_v<StructureType, Laboratory>) { return Structure::StructureClass::Laboratory; }
	else if constexpr (std::is_same_v<StructureType, MaintenanceFacility>) { return Structure::StructureClass::Maintenance; }
	else if constexpr (std::is_same_v<StructureType, MedicalCenter>) { return Structure::StructureClass::MedicalCenter; }
	else if constexpr (std::is_same_v<StructureType, MineFacility>) { return Structure::StructureClass::Mine; }
	else if constexpr (std::is_same_v<StructureType, MineShaft>) { return Structure::StructureClass::Undefined; }
	else if constexpr (std::is_same_v<StructureType, Nursery>) { return Structure::StructureClass::Nursery; }
	else if constexpr (std::is_same_v<StructureType, OreRefining>) { return Structure::StructureClass::Smelter; }
	else if constexpr (std::is_same_v<StructureType, Park>) { return Structure::StructureClass::Park; }
	else if constexpr (std::is_same_v<StructureType, PowerStructure>) { return Structure::StructureClass::EnergyProduction; }
	else if constexpr (std::is_same_v<StructureType, RecreationCenter>) { return Structure::StructureClass::RecreationCenter; }
	else if constexpr (std::is_same_v<StructureType, Recycling>) { return Structure::StructureClass::Recycling; }
	else if constexpr (std::is_same_v<StructureType, RedLightDistrict>) { return Structure::StructureClass::Residence; }
	else if constexpr (std::is_same_v<StructureType, Residence>) { return Structure::StructureClass::Residence; }
	else if constexpr (std::is_same_v<StructureType, Road>) { return Structure::StructureClass::Road; }
	else if constexpr (std::is_same_v<StructureType, RobotCommand>) { return Structure::StructureClass::RobotCommand; }
	else if constexpr (std::is_same_v<StructureType, SeedFactory>) { return Structure::StructureClass::Factory; }
	else if constexpr (std::is_same_v<StructureType, SeedLander>) { return Structure::StructureClass::Lander; }
	else if constexpr (std::is_same_v<StructureType, SeedPower>) { return Structure::StructureClass::EnergyProduction; }
	else if constexpr (std::is_same_v<StructureType, SeedSmelter>) { return Structure::StructureClass::Smelter; }
	else if constexpr (std::is_same_v<StructureType, Smelter>) { return Structure::StructureClass::Smelter; }
	else if constexpr (std::is_same_v<StructureType, SolarPanelArray>) { return Structure::StructureClass::EnergyProduction; }
	else if constexpr (std::is_same_v<StructureType, SolarPlant>) { return Structure::StructureClass::EnergyProduction; }
	else if constexpr (std::is_same_v<StructureType, StorageTanks>) { return Structure::StructureClass::Storage; }
	else if constexpr (std::is_same_v<StructureType, SurfaceFactory>) { return Structure::StructureClass::Factory; }
	else if constexpr (std::is_same_v<StructureType, SurfacePolice>) { return Structure::StructureClass::SurfacePolice; }
	else if constexpr (std::is_same_v<StructureType, Tube>) { return Structure::StructureClass::Tube; }
	else if constexpr (std::is_same_v<StructureType, UndergroundFactory>) { return Structure::StructureClass::Factory; }
	else if constexpr (std::is_same_v<StructureType, UndergroundPolice>) { return Structure::StructureClass::UndergroundPolice; }
	else if constexpr (std::is_same_v<StructureType, University>) { return Structure::StructureClass::University; }
	else if constexpr (std::is_same_v<StructureType, Warehouse>) { return Structure::StructureClass::Warehouse; }
	else { static_assert(dependent_false<StructureType>, "Unknown type"); }
}


/**
 * Handles structure updating and resource management for structures.
 *
 * Keeps track of which structures are operational, idle and disabled.
 */
class StructureManager
{
public:
	void addStructure(Structure& structure, Tile& tile);
	void removeStructure(Structure& structure);

	template <typename StructureType>
	const std::vector<StructureType*> getStructures()
	{
		// Get list of structures with same function
		const auto& sameClassStructures = structureList(structureTypeToClass<StructureType>());

		std::vector<StructureType*> output;
		// Filter for instances of the exact type parameter
		for (auto structure : sameClassStructures)
		{
			StructureType* derivedStructure = dynamic_cast<StructureType*>(structure);
			if (derivedStructure)
			{
				output.push_back(derivedStructure);
			}
		}
		return output;
	}

	const StructureList& structureList(Structure::StructureClass structureClass);
	StructureList allStructures();

	Tile& tileFromStructure(Structure* structure);

	void disconnectAll();
	void dropAllStructures();

	int count() const;

	int getCountInState(Structure::StructureClass structureClass, StructureState state);

	const StructureList& agingStructures() const { return mAgingStructures; }
	const StructureList& newlyBuiltStructures() const { return mNewlyBuiltStructures; }
	const StructureList& structuresWithCrime() const { return mStructuresWithCrime; }

	int disabled();
	int destroyed();

	bool CHAPAvailable();

	void updateEnergyProduction();
	void updateEnergyConsumed();
	int totalEnergyProduction() const { return mTotalEnergyOutput; }
	int totalEnergyUsed() const { return mTotalEnergyUsed; }
	int totalEnergyAvailable() const { return mTotalEnergyOutput - mTotalEnergyUsed; }

	void assignColonistsToResidences(PopulationPool&);
	void assignScientistsToResearchFacilities(PopulationPool&);

	void update(const StorableResources&, PopulationPool&);

	NAS2D::Xml::XmlElement* serialize(std::map<const Robot*, int> robotToIdMap);

private:
	using StructureTileTable = std::map<Structure*, Tile*>;
	using StructureClassTable = std::map<Structure::StructureClass, StructureList>;

	void updateStructures(const StorableResources&, PopulationPool&, StructureList&);

	bool structureConnected(Structure* structure);

	StructureTileTable mStructureTileTable; /**< List mapping Structures to a particular tile. */
	StructureClassTable mStructureLists; /**< Map containing all of the structure list types available. */

	StructureList mAgingStructures;
	StructureList mNewlyBuiltStructures;
	StructureList mStructuresWithCrime;

	int mTotalEnergyOutput = 0; /**< Total energy output of all energy producers in the structure list. */
	int mTotalEnergyUsed = 0;
};
