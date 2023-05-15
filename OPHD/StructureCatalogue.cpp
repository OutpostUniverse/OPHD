#include "StructureCatalogue.h"

#include "StorableResources.h"
#include "MapObjects/Structures.h"

#include <string>
#include <stdexcept>


namespace
{
	std::map<StructureID, StorableResources> buildRecycleValueTable(int recoveryPercent);


	// RESOURCES: CommonMetals | CommonMinerals | RareMetals | RareMinerals
	const std::map<StructureID, StorableResources> StructureCostTable =
	{{
		{SID_NONE, {}},
		{SID_AGRIDOME, {10, 10, 2, 2}},
		{SID_CHAP, {15, 10, 6, 6}},
		{SID_COMMAND_CENTER, {100, 75, 75, 40}},
		{SID_COMMERCIAL, {8, 5, 5, 0}},
		{SID_COMM_TOWER, {5, 5, 5, 0}},
		{SID_FUSION_REACTOR, {30, 25, 20, 10}},
		{SID_HOT_LABORATORY, {20, 10, 6, 5}},
		{SID_LABORATORY, {20, 10, 6, 5}},
		{SID_MAINTENANCE_FACILITY, {10, 5, 0, 0}},
		{SID_MEDICAL_CENTER, {8, 5, 3, 3}},
		{SID_NURSERY, {8, 5, 0, 0}},
		{SID_PARK, {6, 6, 0, 0}},
		{SID_SURFACE_POLICE, {8, 5, 8, 2}},
		{SID_UNDERGROUND_POLICE, {8, 5, 8, 2}},
		{SID_RECREATION_CENTER, {10, 8, 4, 0}},
		{SID_RECYCLING, {10, 6, 8, 3}},
		{SID_RED_LIGHT_DISTRICT, {15, 12, 10, 3}},
		{SID_RESIDENCE, {8, 8, 0, 0}},
		{SID_ROAD, {5, 5, 0, 0}},
		{SID_ROBOT_COMMAND, {20, 15, 10, 5}},
		{SID_SMELTER, {20, 15, 5, 5}},
		{SID_SOLAR_PANEL1, {10, 20, 5, 5}},
		{SID_SOLAR_PLANT, {30, 20, 20, 20}},
		{SID_STORAGE_TANKS, {5, 5, 0, 0}},
		{SID_SURFACE_FACTORY, {20, 10, 10, 5}},
		{SID_UNDERGROUND_FACTORY, {20, 10, 10, 5}},
		{SID_UNIVERSITY, {8, 8, 5, 5}},
		{SID_WAREHOUSE, {5, 5, 0, 0}},
	}};

	/**	Currently set at 90% but this should probably be
	 *	lowered for actual gameplay with modifiers to improve efficiency. */
	const int DefaultRecyclePercent = 90;

	std::map<StructureID, StorableResources> StructureRecycleValueTable;

	const std::map<StructureID, PopulationRequirements> PopulationRequirementsTable = {
		{SID_NONE, {}},
		{SID_AGRIDOME, {1, 0}},
		{SID_CHAP, {2, 0}},
		{SID_COMMERCIAL, {1, 0}},
		{SID_FUSION_REACTOR, {1, 2}},
		{SID_HOT_LABORATORY, {0, 0}},
		{SID_LABORATORY, {0, 0}},
		{SID_MEDICAL_CENTER, {1, 2}},
		{SID_NURSERY, {1, 1}},
		{SID_PARK, {1, 0}},
		{SID_SURFACE_POLICE, {3, 0}},
		{SID_UNDERGROUND_POLICE, {3, 0}},
		{SID_RECREATION_CENTER, {2, 0}},
		{SID_RECYCLING, {1, 1}},
		{SID_RED_LIGHT_DISTRICT, {2, 0}},
		{SID_ROBOT_COMMAND, {4, 0}},
		{SID_SEED_FACTORY, {2, 0}},
		{SID_SEED_SMELTER, {2, 0}},
		{SID_SMELTER, {4, 0}},
		{SID_SOLAR_PANEL1, {0, 0}},
		{SID_SURFACE_FACTORY, {4, 0}},
		{SID_UNDERGROUND_FACTORY, {2, 0}},
		{SID_UNIVERSITY, {1, 3}},
		{SID_WAREHOUSE, {1, 0}},
	};


	template <typename Value>
	const Value& findOrDefault(const std::map<StructureID, Value>& container, StructureID key)
	{
		const auto it = container.find(key);
		if (it != container.end())
		{
			return it->second;
		}
		return container.at(StructureID::SID_NONE);
	}


	/**
	 * Fills out the recycle value for all structures.
	 */
	std::map<StructureID, StorableResources> buildRecycleValueTable(int recoveryPercent)
	{
		std::map<StructureID, StorableResources> structureRecycleValueTable;

		for (std::size_t i = 0; i < StructureID::SID_COUNT; ++i)
		{
			const auto structureId = static_cast<StructureID>(i);
			structureRecycleValueTable[structureId] = StructureCatalogue::costToBuild(structureId) * recoveryPercent / 100;
		}

		// Set recycling values for landers and automatically built structures.
		// RESOURCES: COMM_MET_ORE, COMM_MIN_ORE, RARE_MET_ORE, RARE_MIN_ORE, COMM_MET, COMM_MIN, RARE_MET, RARE_MIN
		structureRecycleValueTable[StructureID::SID_MINE_FACILITY] = {15, 10, 5, 5};
		structureRecycleValueTable[StructureID::SID_CARGO_LANDER] = {15, 10, 5, 5};
		structureRecycleValueTable[StructureID::SID_COLONIST_LANDER] = {15, 10, 5, 5};
		structureRecycleValueTable[StructureID::SID_SEED_LANDER] = {10, 5, 5, 5};
		structureRecycleValueTable[StructureID::SID_SEED_FACTORY] = {15, 10, 5, 5};
		structureRecycleValueTable[StructureID::SID_SEED_POWER] = {15, 10, 5, 5};
		structureRecycleValueTable[StructureID::SID_SEED_SMELTER] = {15, 10, 5, 5};

		return structureRecycleValueTable;
	}
}


/**
 * Initializes StructureCatalogue.
 */
void StructureCatalogue::init()
{
	StructureRecycleValueTable = buildRecycleValueTable(DefaultRecyclePercent);
}


/**
 * Gets a new Structure object given a StructureID.
 * 
 * \param	type	A valid StructureID value.
 * 
 * \return	Pointer to a newly constructed Structure or
 *			\c nullptr if structure type unsupported/invalid.
 */
Structure* StructureCatalogue::get(StructureID type)
{
	Structure* structure = nullptr;

	// This seems like a naive approach... I usually see these implemented as the base
	// object type has a static function that is used as an interface to instantiate
	// derived types.
	switch (type)
	{
		case StructureID::SID_AGRIDOME:
			structure = new Agridome();
			break;

		case StructureID::SID_AIR_SHAFT:
			structure = new AirShaft();
			break;

		case StructureID::SID_CARGO_LANDER: // only here for loading games
			structure = new CargoLander(nullptr);
			break;

		case StructureID::SID_CHAP:
			structure = new CHAP();
			break;

		case StructureID::SID_COLONIST_LANDER: // only here for loading games
			structure = new ColonistLander(nullptr);
			break;

		case StructureID::SID_COMMAND_CENTER:
			structure = new CommandCenter();
			break;

		case StructureID::SID_COMMERCIAL:
			structure = new Commercial();
			break;

		case StructureID::SID_COMM_TOWER:
			structure = new CommTower();
			break;

		case StructureID::SID_FUSION_REACTOR:
			structure = new FusionReactor();
			break;

		case StructureID::SID_HOT_LABORATORY:
			structure = new HotLaboratory();
			break;

		case StructureID::SID_LABORATORY:
			structure = new Laboratory();
			break;

		case StructureID::SID_MAINTENANCE_FACILITY:
			structure = new MaintenanceFacility();
			break;

		case StructureID::SID_MEDICAL_CENTER:
			structure = new MedicalCenter();
			break;

		case StructureID::SID_MINE_FACILITY: // only here for loading games
			structure = new MineFacility(nullptr);
			break;

		case StructureID::SID_MINE_SHAFT: // only here for loading games
			structure = new MineShaft();
			break;

		case StructureID::SID_NURSERY:
			structure = new Nursery();
			break;

		case StructureID::SID_PARK:
			structure = new Park();
			break;

		case StructureID::SID_ROAD:
			structure = new Road();
			break;

		case StructureID::SID_SURFACE_POLICE:
			structure = new SurfacePolice();
			break;

		case StructureID::SID_UNDERGROUND_POLICE:
			structure = new UndergroundPolice();
			break;

		case StructureID::SID_RECREATION_CENTER:
			structure = new RecreationCenter();
			break;

		case StructureID::SID_RECYCLING:
			structure = new Recycling();
			break;

		case StructureID::SID_RED_LIGHT_DISTRICT:
			structure = new RedLightDistrict();
			break;

		case StructureID::SID_RESIDENCE:
			structure = new Residence();
			break;

		case StructureID::SID_ROBOT_COMMAND:
			structure = new RobotCommand();
			break;

		case StructureID::SID_SEED_FACTORY:
			structure = new SeedFactory();
			break;

		case StructureID::SID_SEED_LANDER: // only here for loading games
			structure = new SeedLander({0, 0});
			break;

		case StructureID::SID_SEED_POWER:
			structure = new SeedPower();
			break;

		case StructureID::SID_SEED_SMELTER:
			structure = new SeedSmelter();
			break;

		case StructureID::SID_SMELTER:
			structure = new Smelter();
			break;

		case StructureID::SID_SOLAR_PANEL1:
			structure = new SolarPanelArray();
			break;

		case StructureID::SID_SOLAR_PLANT:
			structure = new SolarPlant();
			break;

		case StructureID::SID_STORAGE_TANKS:
			structure = new StorageTanks();
			break;

		case StructureID::SID_SURFACE_FACTORY:
			structure = new SurfaceFactory();
			break;

		case StructureID::SID_UNDERGROUND_FACTORY:
			structure = new UndergroundFactory();
			break;

		case StructureID::SID_UNIVERSITY:
			structure = new University();
			break;

		case StructureID::SID_WAREHOUSE:
			structure = new Warehouse();
			break;


		case StructureID::SID_TUBE:
			break;

		case StructureID::SID_NONE:
			break;

		case StructureID::SID_COUNT:
			break;
	}

	if (!structure)
	{
		throw std::runtime_error("StructureCatalogue::get(): Unsupported structure type: " + std::to_string(type));
	}

	structure->setPopulationRequirements(StructureCatalogue::populationRequirements(type));

	return structure;
}


/**
 * Gets the population required to operate a given Structure.
 * 
 * \param	type	A valid StructureID value.
 */
const PopulationRequirements& StructureCatalogue::populationRequirements(StructureID type)
{
	return findOrDefault(PopulationRequirementsTable, type);
}


/**
 * Gets the cost in resources required to build a given Structure.
 * 
 * \param	type	A valid StructureID value.
 */
const StorableResources& StructureCatalogue::costToBuild(StructureID type)
{
	return findOrDefault(StructureCostTable, type);
}


/**
 * Gets the recycling value of a specified structure type.
 * 
 * \param	type	A valid StructureID value.
 */
const StorableResources& StructureCatalogue::recyclingValue(StructureID type)
{
	return findOrDefault(StructureRecycleValueTable, type);
}


/**
 * Indicates that the source ResourcePool has enough resources to accommodate
 * the resource requirements of the specificed structure.
 */
bool StructureCatalogue::canBuild(const StorableResources& source, StructureID type)
{
	return StructureCatalogue::costToBuild(type) <= source;
}
