#include "StructureCatalog.h"

#include "MapObjects/Structures.h"
#include "IOHelper.h"

#include <libOPHD/EnumStructureID.h>
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
		std::string{"Agricultural Dome"},
		std::string{"Air Shaft"},
		std::string{"Cargo Lander"},
		std::string{"CHAP Facility"},
		std::string{"Colonist Lander"},
		std::string{"Command Center"},
		std::string{"Commercial"},
		std::string{"Communications Tower"},
		std::string{"Fusion Reactor"},
		std::string{"Hot Laboratory"},
		std::string{"Laboratory"},
		std::string{"Medical Center"},
		std::string{"Mine Facility"},
		std::string{"Mine Shaft"},
		std::string{"Nursery"},
		std::string{"Park / Reservoir"},
		std::string{"Recreation Center"},
		std::string{"Red Light District"},
		std::string{"Residential Facility"},
		std::string{"Road"},
		std::string{"Robot Command"},
		std::string{"SEED Factory"},
		std::string{"SEED Lander"},
		std::string{"SEED Power"},
		std::string{"SEED Smelter"},
		std::string{"Smelter"},
		std::string{"Solar Panel Array"},
		std::string{"Solar Powersat Receiver Array"},
		std::string{"Storage Tanks"},
		std::string{"Surface Factory"},
		std::string{"Police"},
		std::string{"Tube"},
		std::string{"Underground Factory"},
		std::string{"Police (UG)"},
		std::string{"University"},
		std::string{"Warehouse"},
		std::string{"Recycling Facility"},
		std::string{"Maintenance Facility"},
	};

	constexpr auto structureIdCount = static_cast<std::size_t>(StructureID::Count);

	/**	Currently set at 90% but this should probably be
	 *	lowered for actual gameplay with modifiers to improve efficiency. */
	const int defaultRecyclePercent = 90;

	std::vector<StructureType> structureTypes;
	std::vector<StorableResources> recycleValueTable;


	std::size_t typeIndex(StructureID id)
	{
		return static_cast<std::size_t>(id);
	}


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
		structureRecycleValueTable[typeIndex(StructureID::MineFacility)] = {15, 10, 5, 5};
		structureRecycleValueTable[typeIndex(StructureID::CargoLander)] = {15, 10, 5, 5};
		structureRecycleValueTable[typeIndex(StructureID::ColonistLander)] = {15, 10, 5, 5};
		structureRecycleValueTable[typeIndex(StructureID::SeedLander)] = {10, 5, 5, 5};
		structureRecycleValueTable[typeIndex(StructureID::SeedFactory)] = {15, 10, 5, 5};
		structureRecycleValueTable[typeIndex(StructureID::SeedPower)] = {15, 10, 5, 5};
		structureRecycleValueTable[typeIndex(StructureID::SeedSmelter)] = {15, 10, 5, 5};

		return structureRecycleValueTable;
	}


	std::vector<StructureType> loadStructureTypes(const std::string& filePath)
	{
		const auto document = openXmlFile(filePath, "Structures");
		const auto& structuresElement = *document.firstChildElement("Structures");

		const auto requiredFields = std::vector<std::string>{"Name", "ImagePath"};
		const auto optionalFields = std::vector<std::string>{"Description", "TurnsToBuild", "MaxAge", "RequiredWorkers", "RequiredScientists", "Priority", "EnergyRequired", "EnergyProduced", "SolarEnergyProduced", "RawOreStorageCapacity", "OreStorageCapacity", "FoodProduced", "FoodStorageCapacity", "ResidentialCapacity", "BioWasteStorageCapacity", "BioWasteProcessingCapacity", "RobotCommandCapacity", "CommRange", "PoliceRange", "IntegrityDecayRate", "PopulationRequirements", "ResourceRequirements", "IsSelfSustained", "IsRepairable", "IsChapRequired", "IsCrimeTarget"};

		std::vector<StructureType> loadedStructureTypes;
		for (const auto* structureElement = structuresElement.firstChildElement(); structureElement; structureElement = structureElement->nextSiblingElement())
		{
			const auto dictionary = NAS2D::attributesToDictionary(*structureElement);
			NAS2D::reportMissingOrUnexpected(dictionary.keys(), requiredFields, optionalFields);

			loadedStructureTypes.push_back({
				dictionary.get("Name"),
				dictionary.get("Description", std::string{}),
				dictionary.get("ImagePath"),
				readResourcesOptional(*structureElement, "BuildCost"),
				readResourcesOptional(*structureElement, "OperationalCost"),
				{
					dictionary.get<int>("RequiredWorkers", 0),
					dictionary.get<int>("RequiredScientists", 0),
				},
				dictionary.get<int>("TurnsToBuild", 0),
				dictionary.get<int>("MaxAge", 0),
				dictionary.get<int>("Priority", 0),
				dictionary.get<int>("EnergyRequired", 0),
				dictionary.get<int>("EnergyProduced", 0),
				dictionary.get<int>("SolarEnergyProduced", 0),
				dictionary.get<int>("RawOreStorageCapacity", 0),
				dictionary.get<int>("OreStorageCapacity", 0),
				dictionary.get<int>("FoodProduced", 0),
				dictionary.get<int>("FoodStorageCapacity", 0),
				dictionary.get<int>("ResidentialCapacity", 0),
				dictionary.get<int>("BioWasteStorageCapacity", 0),
				dictionary.get<int>("BioWasteProcessingCapacity", 0),
				dictionary.get<int>("RobotCommandCapacity", 0),
				dictionary.get<int>("CommRange", 0),
				dictionary.get<int>("PoliceRange", 0),
				dictionary.get<int>("IntegrityDecayRate", 1),
				dictionary.get<bool>("IsSelfSustained", false),
				dictionary.get<bool>("IsRepairable", true),
				dictionary.get<bool>("IsChapRequired", true),
				dictionary.get<bool>("IsCrimeTarget", false),
			});
		}
		return loadedStructureTypes;
	}


	void verifyStructureTypeOrder()
	{
		if (structureTypes.size() != structureIdCount)
		{
			throw std::runtime_error("Unexpected number of StructureType entries: Read: " + std::to_string(structureTypes.size()) + " Expected: " + std::to_string(structureIdCount));
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
	if (structureTypeIndex >= structureTypes.size())
	{
		throw std::runtime_error("StructureCatalog::getType called with invalid index: " + std::to_string(structureTypeIndex) + " of " + std::to_string(structureTypes.size()));
	}
	return structureTypes.at(structureTypeIndex);
}


/**
 * Gets a new Structure object given a StructureID.
 *
 * \param	structureId	A valid StructureID value.
 *
 * \return	Pointer to a newly constructed Structure
 * \throw	std::runtime_error if the StructureID is unsupported/invalid
 */
Structure* StructureCatalog::create(StructureID structureId, Tile& tile)
{
	Structure* structure = nullptr;

	switch (structureId)
	{
		case StructureID::Agridome:
			structure = new Agridome(tile);
			break;

		case StructureID::AirShaft:
			structure = new AirShaft(tile);
			break;

		case StructureID::CargoLander:
			structure = new CargoLander(tile);
			break;

		case StructureID::ColonistLander:
			structure = new ColonistLander(tile);
			break;

		case StructureID::CommandCenter:
			structure = new CommandCenter(tile);
			break;

		case StructureID::CommTower:
			structure = new CommTower(tile);
			break;

		case StructureID::HotLaboratory:
			structure = new HotLaboratory(tile);
			break;

		case StructureID::Laboratory:
			structure = new Laboratory(tile);
			break;

		case StructureID::MaintenanceFacility:
			structure = new MaintenanceFacility(tile);
			break;

		case StructureID::MineFacility:
			structure = new MineFacility(tile);
			break;

		case StructureID::MineShaft:
			structure = new MineShaft(tile);
			break;

		case StructureID::Road:
			structure = new Road(tile);
			break;

		case StructureID::Recycling:
			structure = new Recycling(tile);
			break;

		case StructureID::Residence:
			structure = new Residence(tile);
			break;

		case StructureID::SeedFactory:
			structure = new SeedFactory(tile);
			break;

		case StructureID::SeedLander:
			structure = new SeedLander(tile);
			break;

		case StructureID::StorageTanks:
			structure = new StorageTanks(tile);
			break;

		case StructureID::SurfaceFactory:
			structure = new SurfaceFactory(tile);
			break;

		case StructureID::UndergroundFactory:
			structure = new UndergroundFactory(tile);
			break;

		case StructureID::Warehouse:
			structure = new Warehouse(tile);
			break;

		case StructureID::Tube:
			structure = new Tube(tile);
			break;

		case StructureID::SeedSmelter:
		case StructureID::Smelter:
			structure = new OreRefining(structureId, tile);
			break;

		case StructureID::Chap:
		case StructureID::Commercial:
		case StructureID::FusionReactor:
		case StructureID::MedicalCenter:
		case StructureID::Nursery:
		case StructureID::Park:
		case StructureID::SurfacePolice:
		case StructureID::UndergroundPolice:
		case StructureID::RecreationCenter:
		case StructureID::RedLightDistrict:
		case StructureID::RobotCommand:
		case StructureID::SeedPower:
		case StructureID::SolarPanel1:
		case StructureID::SolarPlant:
		case StructureID::University:
			structure = new Structure(structureId, tile);
			break;


		case StructureID::None:
		case StructureID::Count:
			break;
	}

	if (!structure)
	{
		throw std::runtime_error("StructureCatalog::create(): Unsupported structure type: " + std::to_string(static_cast<std::size_t>(structureId)));
	}

	return structure;
}


Structure* StructureCatalog::create(std::size_t structureTypeIndex, Tile& tile)
{
	return create(static_cast<StructureID>(structureTypeIndex), tile);
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


bool StructureCatalog::canBuild(std::size_t structureTypeIndex, const StorableResources& source)
{
	return costToBuild(structureTypeIndex) <= source;
}
