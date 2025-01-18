#include "StructureCatalogue.h"

#include "StorableResources.h"
#include "MapObjects/Structures.h"
#include "MapObjects/StructureType.h"
#include "IOHelper.h"

#include <libOPHD/XmlSerializer.h>

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/ParserHelper.h>

#include <string>
#include <stdexcept>


namespace
{
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

		for (std::size_t i = 1; i < StructureID::SID_COUNT; ++i)
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


	std::vector<StructureType> loadStructureTypes(const std::string& filePath)
	{
		const auto document = openXmlFile(filePath, "Structures");
		const auto& structuresElement = *document.firstChildElement("Structures");

		const auto requiredFields = std::vector<std::string>{"Name", "ImagePath", "TurnsToBuild", "MaxAge"};
		const auto optionalFields = std::vector<std::string>{"RequiredWorkers", "RequiredScientists", "Priority", "EnergyRequired", "EnergyProduced", "FoodProduced", "FoodStorageCapacity", "OreStorageCapacity", "IntegrityDecayRate", "PopulationRequirements", "ResourceRequirements", "IsSelfSustained", "IsRepairable", "IsChapRequired", "IsCrimeTarget"};

		std::vector<StructureType> structureTypes;
		for (const auto* structureElement = structuresElement.firstChildElement(); structureElement; structureElement = structureElement->nextSiblingElement())
		{
			const auto dictionary = NAS2D::attributesToDictionary(*structureElement);
			NAS2D::reportMissingOrUnexpected(dictionary.keys(), requiredFields, optionalFields);

			structureTypes.push_back({
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
				dictionary.get<int>("IntegrityDecayRate"),
				dictionary.get<bool>("IsSelfSustained"),
				dictionary.get<bool>("IsRepairable"),
				dictionary.get<bool>("IsChapRequired"),
				dictionary.get<bool>("IsCrimeTarget"),
			});
		}
		return structureTypes;
	}


	std::vector<StructureType> structureTypes;


	const StructureType& findStructureType(const std::string& name)
	{
		for (const auto& structureType : structureTypes)
		{
			if (structureType.name == name)
			{
				return structureType;
			}
		}
		throw std::runtime_error("StructureType not found: " + name);
	}


	auto buildStructureTypeLookup()
	{
		std::map<StructureID, const StructureType&> idToType;
		for (std::size_t i = 1; i < StructureID::SID_COUNT; ++i)
		{
			const auto structureId = static_cast<StructureID>(i);
			const auto& structureName = StructureName(structureId);
			idToType.emplace(structureId, findStructureType(structureName));
		}
		return idToType;
	}


	std::map<StructureID, const StructureType&> idToType;
}


/**
 * Initializes StructureCatalogue.
 */
void StructureCatalogue::init()
{
	structureTypes = loadStructureTypes("StructureTypes.xml");
	idToType = buildStructureTypeLookup();
	StructureRecycleValueTable = buildRecycleValueTable(DefaultRecyclePercent);
}


const StructureType& StructureCatalogue::getType(StructureID type)
{
	return idToType.at(type);
}


/**
 * Gets a new Structure object given a StructureID.
 *
 * \param	type	A valid StructureID value.
 *
 * \return	Pointer to a newly constructed Structure
 * \throw	std::runtime_error if the StructureID is unsupported/invalid
 */
Structure* StructureCatalogue::get(StructureID type, Tile* tile)
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
			structure = new CargoLander(tile);
			break;

		case StructureID::SID_CHAP:
			structure = new CHAP();
			break;

		case StructureID::SID_COLONIST_LANDER: // only here for loading games
			structure = new ColonistLander(*tile);
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

	return structure;
}


/**
 * Gets the cost in resources required to build a given Structure.
 *
 * \param	type	A valid StructureID value.
 */
const StorableResources& StructureCatalogue::costToBuild(StructureID type)
{
	return getType(type).buildCost;
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
 * the resource requirements of the specified structure.
 */
bool StructureCatalogue::canBuild(const StorableResources& source, StructureID type)
{
	return StructureCatalogue::costToBuild(type) <= source;
}
