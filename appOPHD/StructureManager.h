#pragma once

#include "MapObjects/Structure.h"
#include "MapObjects/Structures.h"

#include <map>
#include <vector>


namespace NAS2D
{
	namespace Xml
	{
		class XmlElement;
	}
}

class Tile;
class TileMap;
class PopulationPool;
struct StorableResources;
struct MapCoordinate;


template <typename T> constexpr bool dependent_false = false;

template <typename StructureType>
constexpr Structure::StructureClass structureTypeToClass() {
	if constexpr (std::is_same_v<StructureType, Agridome>) { return Structure::StructureClass::FoodProduction; }
	else if constexpr (std::is_same_v<StructureType, AirShaft>) { return Structure::StructureClass::Tube; }
	else if constexpr (std::is_same_v<StructureType, CargoLander>) { return Structure::StructureClass::Lander; }
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
	else if constexpr (std::is_same_v<StructureType, ResearchFacility>) { return Structure::StructureClass::Laboratory; }
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
	StructureManager();

	void addStructure(Structure& structure, Tile& tile);
	void removeStructure(Structure& structure);

	template <typename StructureType>
	const std::vector<StructureType*> getStructures() const
	{
		// Get list of structures with same function
		const auto& sameClassStructures = structureList(structureTypeToClass<StructureType>());

		std::vector<StructureType*> output;
		// Filter for instances of the exact type parameter
		for (auto* structure : sameClassStructures)
		{
			StructureType* derivedStructure = dynamic_cast<StructureType*>(structure);
			if (derivedStructure)
			{
				output.push_back(derivedStructure);
			}
		}
		return output;
	}

	const StructureList& structureList(Structure::StructureClass structureClass) const;
	StructureList allStructures() const;

	Tile& tileFromStructure(const Structure* structure) const;

	std::vector<MapCoordinate> operationalCommandCenterPositions() const;

	void updateConnectedness(TileMap& tileMap);
	std::vector<Tile*> getConnectednessOverlay() const;

	void dropAllStructures();

	int count() const;

	int getCountInState(Structure::StructureClass structureClass, StructureState state) const;

	template <typename StructureType>
	unsigned int countInState(StructureState state) const
	{
		unsigned int count = 0;
		for (const auto* structure : getStructures<StructureType>())
		{
			if (structure->state() == state)
			{
				++count;
			}
		}
		return count;
	}

	const StructureList& agingStructures() const { return mAgingStructures; }
	const StructureList& newlyBuiltStructures() const { return mNewlyBuiltStructures; }
	const StructureList& structuresWithCrime() const { return mStructuresWithCrime; }

	int disabled() const;
	int destroyed() const;

	bool CHAPAvailable() const;

	void updateEnergyProduction();
	void updateEnergyConsumed();
	int totalEnergyProduction() const { return mTotalEnergyOutput; }
	int totalEnergyUsed() const { return mTotalEnergyUsed; }
	int totalEnergyAvailable() const { return mTotalEnergyOutput - mTotalEnergyUsed; }

	int totalFoodStorageCapacity() const;

	void assignColonistsToResidences(PopulationPool&);
	void assignScientistsToResearchFacilities(PopulationPool&);

	void update(const StorableResources&, PopulationPool&);

	NAS2D::Xml::XmlElement* serialize() const;

private:
	using StructureTileTable = std::map<Structure*, Tile*>;
	using StructureClassTable = std::map<Structure::StructureClass, StructureList>;

	void disconnectAll();

	void updateStructures(const StorableResources&, PopulationPool&, StructureList&);

	StructureTileTable mStructureTileTable; /**< List mapping Structures to a particular tile. */
	StructureClassTable mStructureLists; /**< Map containing all of the structure list types available. */

	StructureList mAgingStructures;
	StructureList mNewlyBuiltStructures;
	StructureList mStructuresWithCrime;

	int mTotalEnergyOutput = 0; /**< Total energy output of all energy producers in the structure list. */
	int mTotalEnergyUsed = 0;
};
