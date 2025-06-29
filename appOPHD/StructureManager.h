#pragma once

#include "MapObjects/Structure.h"
#include "MapObjects/Structures.h"

#include <map>
#include <vector>
#include <concepts>


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
constexpr StructureClass structureTypeToClass() {
	if constexpr (std::is_same_v<StructureType, Agridome>) { return StructureClass::FoodProduction; }
	else if constexpr (std::is_same_v<StructureType, AirShaft>) { return StructureClass::Tube; }
	else if constexpr (std::is_same_v<StructureType, CargoLander>) { return StructureClass::Lander; }
	else if constexpr (std::is_same_v<StructureType, ColonistLander>) { return StructureClass::Lander; }
	else if constexpr (std::is_same_v<StructureType, CommandCenter>) { return StructureClass::Command; }
	else if constexpr (std::is_same_v<StructureType, CommTower>) { return StructureClass::Communication; }
	else if constexpr (std::is_same_v<StructureType, Factory>) { return StructureClass::Factory; }
	else if constexpr (std::is_same_v<StructureType, FoodProduction>) { return StructureClass::FoodProduction; }
	else if constexpr (std::is_same_v<StructureType, FusionReactor>) { return StructureClass::EnergyProduction; }
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
	else if constexpr (std::is_same_v<StructureType, SeedPower>) { return StructureClass::EnergyProduction; }
	else if constexpr (std::is_same_v<StructureType, SolarPanelArray>) { return StructureClass::EnergyProduction; }
	else if constexpr (std::is_same_v<StructureType, SolarPlant>) { return StructureClass::EnergyProduction; }
	else if constexpr (std::is_same_v<StructureType, StorageTanks>) { return StructureClass::Storage; }
	else if constexpr (std::is_same_v<StructureType, SurfaceFactory>) { return StructureClass::Factory; }
	else if constexpr (std::is_same_v<StructureType, Tube>) { return StructureClass::Tube; }
	else if constexpr (std::is_same_v<StructureType, UndergroundFactory>) { return StructureClass::Factory; }
	else if constexpr (std::is_same_v<StructureType, Warehouse>) { return StructureClass::Warehouse; }
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

	template <typename StructureType>
	requires std::derived_from<StructureType, Structure> && std::constructible_from<StructureType, Tile*>
	StructureType& create(Tile& tile)
	{
		auto& structure = *new StructureType(&tile);
		addStructure(structure, tile);
		return structure;
	}

	template <typename StructureType>
	requires std::derived_from<StructureType, Structure> && std::constructible_from<StructureType>
	StructureType& create(Tile& tile)
	{
		auto& structure = *new StructureType();
		addStructure(structure, tile);
		return structure;
	}

	Structure& create(StructureID structureId, Tile& tile);

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

	const StructureList& structureList(StructureClass structureClass) const;
	StructureList allStructures() const;

	Tile& tileFromStructure(const Structure* structure) const;

	std::vector<MapCoordinate> operationalCommandCenterPositions() const;

	void updateConnectedness(TileMap& tileMap);
	std::vector<Tile*> getConnectednessOverlay() const;

	void dropAllStructures();

	int count() const;

	int getCountInState(StructureClass structureClass, StructureState state) const;

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
	using StructureClassTable = std::map<StructureClass, StructureList>;

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
