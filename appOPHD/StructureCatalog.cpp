#include "StructureCatalog.h"

#include "MapObjects/Structures.h"
#include "IOHelper.h"
#include "Constants/Strings.h"

#include <libOPHD/MapObjects/StructureType.h>
#include <libOPHD/StorableResources.h>
#include <libOPHD/XmlSerializer.h>

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/ParserHelper.h>

#include <array>
#include <vector>
#include <stdexcept>


namespace
{
	static const std::array StructureNameTable =
	{
		std::string{"Null"},
		constants::Agridome,
		constants::AirShaft,
		constants::CargoLander,
		constants::Chap,
		constants::ColonistLander,
		constants::CommandCenter,
		constants::Commercial,
		constants::CommTower,
		constants::FusionReactor,
		constants::HotLaboratory,
		constants::Laboratory,
		constants::MedicalCenter,
		constants::MineFacility,
		constants::MineShaft,
		constants::Nursery,
		constants::Park,
		constants::RecreationCenter,
		constants::RedLightDistrict,
		constants::Residence,
		constants::Road,
		constants::RobotCommand,
		constants::SeedFactory,
		constants::SeedLander,
		constants::SeedPower,
		constants::SeedSmelter,
		constants::Smelter,
		constants::SolarPanel1,
		constants::SolarPlant,
		constants::StorageTanks,
		constants::SurfaceFactory,
		constants::SurfacePolice,
		constants::Tube,
		constants::UndergroundFactory,
		constants::UndergroundPolice,
		constants::University,
		constants::Warehouse,
		constants::Recycling,
		constants::MaintenanceFacility
	};


	/**	Currently set at 90% but this should probably be
	 *	lowered for actual gameplay with modifiers to improve efficiency. */
	const int defaultRecyclePercent = 90;

	std::vector<StructureType> structureTypes;
	std::vector<StorableResources> recycleValueTable;


	/**
	 * Fills out the recycle value for all structures.
	 */
	std::vector<StorableResources> buildRecycleValueTable(int recoveryPercent)
	{
		std::vector<StorableResources> structureRecycleValueTable{StructureCatalog::count()};

		for (std::size_t index = 0; index < StructureCatalog::count(); ++index)
		{
			structureRecycleValueTable[index] = StructureCatalog::getType(index).buildCost * recoveryPercent / 100;
		}

		// Set recycling values for landers and automatically built structures.
		// Resources: {Common Metals, Common Minerals, Rare Metals, Rare Minerals}
		structureRecycleValueTable[StructureID::SID_MINE_FACILITY] = {15, 10, 5, 5};
		structureRecycleValueTable[StructureID::SID_CARGO_LANDER] = {15, 10, 5, 5};
		structureRecycleValueTable[StructureID::SID_COLONIST_LANDER] = {15, 10, 5, 5};
		structureRecycleValueTable[StructureID::SID_SEED_LANDER] = {10, 5, 5, 5};
		structureRecycleValueTable[StructureID::SID_SEED_FACTORY] = {15, 10, 5, 5};
		structureRecycleValueTable[StructureID::SID_SEED_POWER] = {15, 10, 5, 5};
		structureRecycleValueTable[StructureID::SID_SEED_SMELTER] = {15, 10, 5, 5};

		return structureRecycleValueTable;
	}


	std::vector<StructureType> loadStructureTypes(const std::string& filePath)
	{
		const auto document = openXmlFile(filePath, "Structures");
		const auto& structuresElement = *document.firstChildElement("Structures");

		const auto requiredFields = std::vector<std::string>{"Name", "ImagePath", "TurnsToBuild", "MaxAge"};
		const auto optionalFields = std::vector<std::string>{"RequiredWorkers", "RequiredScientists", "Priority", "EnergyRequired", "EnergyProduced", "FoodProduced", "FoodStorageCapacity", "OreStorageCapacity", "CommRange", "PoliceRange", "IntegrityDecayRate", "PopulationRequirements", "ResourceRequirements", "IsSelfSustained", "IsRepairable", "IsChapRequired", "IsCrimeTarget"};

		std::vector<StructureType> loadedStructureTypes;
		for (const auto* structureElement = structuresElement.firstChildElement(); structureElement; structureElement = structureElement->nextSiblingElement())
		{
			const auto dictionary = NAS2D::attributesToDictionary(*structureElement);
			NAS2D::reportMissingOrUnexpected(dictionary.keys(), requiredFields, optionalFields);

			loadedStructureTypes.push_back({
				dictionary.get("Name"),
				dictionary.get("ImagePath"),
				readResources(*structureElement, "BuildCost"),
				readResources(*structureElement, "OperationalCost"),
				{
					dictionary.get<int>("RequiredWorkers"),
					dictionary.get<int>("RequiredScientists"),
				},
				dictionary.get<int>("Priority"),
				dictionary.get<int>("TurnsToBuild"),
				dictionary.get<int>("MaxAge"),
				dictionary.get<int>("EnergyRequired"),
				dictionary.get<int>("EnergyProduced"),
				dictionary.get<int>("FoodProduced"),
				dictionary.get<int>("FoodStorageCapacity"),
				dictionary.get<int>("OreStorageCapacity"),
				dictionary.get<int>("CommRange", 0),
				dictionary.get<int>("PoliceRange", 0),
				dictionary.get<int>("IntegrityDecayRate"),
				dictionary.get<bool>("IsSelfSustained"),
				dictionary.get<bool>("IsRepairable"),
				dictionary.get<bool>("IsChapRequired"),
				dictionary.get<bool>("IsCrimeTarget"),
			});
		}
		return loadedStructureTypes;
	}


	void verifyStructureTypeOrder()
	{
		if (structureTypes.size() != StructureID::SID_COUNT)
		{
			throw std::runtime_error("Unexpected number of StructureType entries: Read: " + std::to_string(structureTypes.size()) + " Expected: " + std::to_string(StructureID::SID_COUNT));
		}

		for (std::size_t i = 0; i < structureTypes.size(); ++i)
		{
			const auto& expectedName = StructureNameTable[i];
			const auto& actualName = structureTypes[i].name;
			if (expectedName != actualName)
			{
				throw std::runtime_error("Unexpected StructureType at index: " + std::to_string(i) + " : " + expectedName + " != " + actualName);
			}
		}
	}
}


/**
 * Initializes StructureCatalog.
 */
void StructureCatalog::init(const std::string& filename)
{
	structureTypes = loadStructureTypes(filename);
	verifyStructureTypeOrder();
	recycleValueTable = buildRecycleValueTable(defaultRecyclePercent);
}


std::size_t StructureCatalog::count()
{
	return structureTypes.size();
}


std::size_t StructureCatalog::typeIndex(StructureID id)
{
	return static_cast<std::size_t>(id);
}


const StructureType& StructureCatalog::getType(StructureID id)
{
	return getType(typeIndex(id));
}


const StructureType& StructureCatalog::getType(std::size_t structureTypeIndex)
{
	return structureTypes.at(structureTypeIndex);
}


/**
 * Gets a new Structure object given a StructureID.
 *
 * \param	id	A valid StructureID value.
 *
 * \return	Pointer to a newly constructed Structure
 * \throw	std::runtime_error if the StructureID is unsupported/invalid
 */
Structure* StructureCatalog::create(StructureID id, Tile& tile)
{
	Structure* structure = nullptr;

	// This seems like a naive approach... I usually see these implemented as the base
	// object type has a static function that is used as an interface to instantiate
	// derived types.
	switch (id)
	{
		case StructureID::SID_AGRIDOME:
			structure = new Agridome(tile);
			break;

		case StructureID::SID_AIR_SHAFT:
			structure = new AirShaft(tile);
			break;

		case StructureID::SID_CARGO_LANDER:
			structure = new CargoLander(tile);
			break;

		case StructureID::SID_CHAP:
			structure = new Structure(StructureID::SID_CHAP, tile);
			break;

		case StructureID::SID_COLONIST_LANDER:
			structure = new ColonistLander(tile);
			break;

		case StructureID::SID_COMMAND_CENTER:
			structure = new CommandCenter(tile);
			break;

		case StructureID::SID_COMMERCIAL:
			structure = new Structure(StructureID::SID_COMMERCIAL, tile);
			break;

		case StructureID::SID_COMM_TOWER:
			structure = new CommTower(tile);
			break;

		case StructureID::SID_FUSION_REACTOR:
			structure = new FusionReactor(tile);
			break;

		case StructureID::SID_HOT_LABORATORY:
			structure = new HotLaboratory(tile);
			break;

		case StructureID::SID_LABORATORY:
			structure = new Laboratory(tile);
			break;

		case StructureID::SID_MAINTENANCE_FACILITY:
			structure = new MaintenanceFacility(tile);
			break;

		case StructureID::SID_MEDICAL_CENTER:
			structure = new Structure(StructureID::SID_MEDICAL_CENTER, tile);
			break;

		case StructureID::SID_MINE_FACILITY:
			structure = new MineFacility(tile);
			break;

		case StructureID::SID_MINE_SHAFT:
			structure = new MineShaft(tile);
			break;

		case StructureID::SID_NURSERY:
			structure = new Structure(StructureID::SID_NURSERY, tile);
			break;

		case StructureID::SID_PARK:
			structure = new Structure(StructureID::SID_PARK, tile);
			break;

		case StructureID::SID_ROAD:
			structure = new Road(tile);
			break;

		case StructureID::SID_SURFACE_POLICE:
			structure = new Structure(StructureID::SID_SURFACE_POLICE, tile);
			break;

		case StructureID::SID_UNDERGROUND_POLICE:
			structure = new Structure(StructureID::SID_UNDERGROUND_POLICE, tile);
			break;

		case StructureID::SID_RECREATION_CENTER:
			structure = new Structure(StructureID::SID_RECREATION_CENTER, tile);
			break;

		case StructureID::SID_RECYCLING:
			structure = new Recycling(tile);
			break;

		case StructureID::SID_RED_LIGHT_DISTRICT:
			structure = new Structure(StructureID::SID_RED_LIGHT_DISTRICT, tile);
			break;

		case StructureID::SID_RESIDENCE:
			structure = new Residence(tile);
			break;

		case StructureID::SID_ROBOT_COMMAND:
			structure = new Structure(StructureID::SID_ROBOT_COMMAND, tile);
			break;

		case StructureID::SID_SEED_FACTORY:
			structure = new SeedFactory(tile);
			break;

		case StructureID::SID_SEED_LANDER:
			structure = new SeedLander(tile);
			break;

		case StructureID::SID_SEED_POWER:
			structure = new SeedPower(tile);
			break;

		case StructureID::SID_SEED_SMELTER:
			structure = new OreRefining(StructureID::SID_SEED_SMELTER, tile);
			break;

		case StructureID::SID_SMELTER:
			structure = new OreRefining(StructureID::SID_SMELTER, tile);
			break;

		case StructureID::SID_SOLAR_PANEL1:
			structure = new SolarPanelArray(tile);
			break;

		case StructureID::SID_SOLAR_PLANT:
			structure = new SolarPlant(tile);
			break;

		case StructureID::SID_STORAGE_TANKS:
			structure = new StorageTanks(tile);
			break;

		case StructureID::SID_SURFACE_FACTORY:
			structure = new SurfaceFactory(tile);
			break;

		case StructureID::SID_UNDERGROUND_FACTORY:
			structure = new UndergroundFactory(tile);
			break;

		case StructureID::SID_UNIVERSITY:
			structure = new Structure(StructureID::SID_UNIVERSITY, tile);
			break;

		case StructureID::SID_WAREHOUSE:
			structure = new Warehouse(tile);
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
		throw std::runtime_error("StructureCatalog::create(): Unsupported structure type: " + std::to_string(id));
	}

	return structure;
}


/**
 * Gets the cost in resources required to build a given Structure.
 *
 * \param	id	A valid StructureID value.
 */
const StorableResources& StructureCatalog::costToBuild(StructureID id)
{
	return getType(id).buildCost;
}


const StorableResources& StructureCatalog::costToBuild(std::size_t structureTypeIndex)
{
	return getType(structureTypeIndex).buildCost;
}


/**
 * Gets the recycling value of a specified structure type.
 *
 * \param	id	A valid StructureID value.
 */
const StorableResources& StructureCatalog::recyclingValue(StructureID id)
{
	return recyclingValue(typeIndex(id));
}


const StorableResources& StructureCatalog::recyclingValue(std::size_t structureTypeIndex)
{
	return recycleValueTable.at(structureTypeIndex);
}


/**
 * Indicates that the source ResourcePool has enough resources to accommodate
 * the resource requirements of the specified structure.
 */
bool StructureCatalog::canBuild(StructureID id, const StorableResources& source)
{
	return costToBuild(id) <= source;
}
