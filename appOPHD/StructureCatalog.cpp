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

#include <map>
#include <array>
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


	std::map<StructureID, StorableResources> buildRecycleValueTable(int recoveryPercent);

	/**	Currently set at 90% but this should probably be
	 *	lowered for actual gameplay with modifiers to improve efficiency. */
	const int DefaultRecyclePercent = 90;

	std::map<StructureID, StorableResources> StructureRecycleValueTable;


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

		for (std::size_t index = 1; index < StructureCatalog::count(); ++index)
		{
			const auto structureId = static_cast<StructureID>(index);
			structureRecycleValueTable[structureId] = StructureCatalog::getType(index).buildCost * recoveryPercent / 100;
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


	std::vector<StructureType> structureTypes;


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
	StructureRecycleValueTable = buildRecycleValueTable(DefaultRecyclePercent);
}


std::size_t StructureCatalog::count()
{
	return structureTypes.size();
}


const StructureType& StructureCatalog::getType(StructureID id)
{
	return getType(static_cast<std::size_t>(id));
}


const StructureType& StructureCatalog::getType(std::size_t index)
{
	return structureTypes.at(index);
}


/**
 * Gets a new Structure object given a StructureID.
 *
 * \param	id	A valid StructureID value.
 *
 * \return	Pointer to a newly constructed Structure
 * \throw	std::runtime_error if the StructureID is unsupported/invalid
 */
Structure* StructureCatalog::create(StructureID id, Tile* tile)
{
	Structure* structure = nullptr;

	// This seems like a naive approach... I usually see these implemented as the base
	// object type has a static function that is used as an interface to instantiate
	// derived types.
	switch (id)
	{
		case StructureID::SID_AGRIDOME:
			structure = new Agridome();
			break;

		case StructureID::SID_AIR_SHAFT:
			structure = new AirShaft();
			break;

		case StructureID::SID_CARGO_LANDER:
			structure = new CargoLander(tile);
			break;

		case StructureID::SID_CHAP:
			structure = new Structure(StructureClass::LifeSupport, StructureID::SID_CHAP);
			break;

		case StructureID::SID_COLONIST_LANDER:
			structure = new ColonistLander(tile);
			break;

		case StructureID::SID_COMMAND_CENTER:
			structure = new CommandCenter();
			break;

		case StructureID::SID_COMMERCIAL:
			structure = new Structure(StructureClass::Commercial, StructureID::SID_COMMERCIAL);
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
			structure = new Structure(StructureClass::MedicalCenter, StructureID::SID_MEDICAL_CENTER);
			break;

		case StructureID::SID_MINE_FACILITY:
			structure = new MineFacility(tile);
			break;

		case StructureID::SID_MINE_SHAFT:
			structure = new MineShaft();
			break;

		case StructureID::SID_NURSERY:
			structure = new Structure(StructureClass::Nursery, StructureID::SID_NURSERY);
			break;

		case StructureID::SID_PARK:
			structure = new Structure(StructureClass::Park, StructureID::SID_PARK);
			break;

		case StructureID::SID_ROAD:
			structure = new Road();
			break;

		case StructureID::SID_SURFACE_POLICE:
			structure = new Structure(StructureClass::SurfacePolice, StructureID::SID_SURFACE_POLICE);
			break;

		case StructureID::SID_UNDERGROUND_POLICE:
			structure = new Structure(StructureClass::UndergroundPolice, StructureID::SID_UNDERGROUND_POLICE);
			break;

		case StructureID::SID_RECREATION_CENTER:
			structure = new Structure(StructureClass::RecreationCenter, StructureID::SID_RECREATION_CENTER);
			break;

		case StructureID::SID_RECYCLING:
			structure = new Recycling();
			break;

		case StructureID::SID_RED_LIGHT_DISTRICT:
			structure = new Structure(StructureClass::Residence, StructureID::SID_RED_LIGHT_DISTRICT);
			break;

		case StructureID::SID_RESIDENCE:
			structure = new Residence();
			break;

		case StructureID::SID_ROBOT_COMMAND:
			structure = new Structure(StructureClass::RobotCommand, StructureID::SID_ROBOT_COMMAND);
			break;

		case StructureID::SID_SEED_FACTORY:
			structure = new SeedFactory();
			break;

		case StructureID::SID_SEED_LANDER:
			structure = new SeedLander(tile);
			break;

		case StructureID::SID_SEED_POWER:
			structure = new SeedPower();
			break;

		case StructureID::SID_SEED_SMELTER:
			structure = new OreRefining(StructureClass::Smelter, StructureID::SID_SEED_SMELTER);
			break;

		case StructureID::SID_SMELTER:
			structure = new OreRefining(StructureClass::Smelter, StructureID::SID_SMELTER);
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
			structure = new Structure(StructureClass::University, StructureID::SID_UNIVERSITY);
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


/**
 * Gets the recycling value of a specified structure type.
 *
 * \param	id	A valid StructureID value.
 */
const StorableResources& StructureCatalog::recyclingValue(StructureID id)
{
	return findOrDefault(StructureRecycleValueTable, id);
}


/**
 * Indicates that the source ResourcePool has enough resources to accommodate
 * the resource requirements of the specified structure.
 */
bool StructureCatalog::canBuild(StructureID id, const StorableResources& source)
{
	return costToBuild(id) <= source;
}
