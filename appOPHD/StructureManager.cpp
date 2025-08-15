#include "StructureManager.h"

#include "Constants/Numbers.h"
#include "GraphWalker.h"
#include "IOHelper.h"
#include "StructureCatalog.h"
#include "Map/Tile.h"
#include "MapObjects/Structure.h"
#include "MapObjects/Robot.h"
#include "MapObjects/StructureState.h"
#include "MapObjects/Structures/CommandCenter.h"
#include "MapObjects/Structures/FoodProduction.h"
#include "MapObjects/Structures/MaintenanceFacility.h"
#include "MapObjects/Structures/MineFacility.h"
#include "MapObjects/Structures/ResearchFacility.h"
#include "MapObjects/Structures/Residence.h"
#include "MapObjects/Structures/Warehouse.h"

#include "States/MapViewStateHelper.h"

#include <libOPHD/EnumDisabledReason.h>
#include <libOPHD/EnumStructureID.h>
#include <libOPHD/MapObjects/StructureType.h>
#include <libOPHD/Population/PopulationPool.h>

#include <NAS2D/ParserHelper.h>
#include <NAS2D/ContainerUtils.h>

#include <algorithm>
#include <sstream>


namespace
{
	PopulationRequirements fillPopulationRequirements(PopulationPool& populationPool, const PopulationRequirements& required)
	{
		return {
			std::min(required.workers, populationPool.availableWorkers()),
			std::min(required.scientists, populationPool.availableScientists())
		};
	}


	NAS2D::Xml::XmlElement* serializeStructure(const Structure& structure)
	{
		const auto& position = structure.xyz();
		NAS2D::Dictionary dictionary =
		{{
			{"x", position.xy.x},
			{"y", position.xy.y},
			{"depth", position.z},
		}};
		dictionary += structure.getDataDict();

		auto* structureElement = dictionaryToAttributes("structure", dictionary);

		const auto& production = structure.production();
		if (!production.isEmpty())
		{
			structureElement->linkEndChild(writeResources(production, "production"));
		}

		const auto& stored = structure.storage();
		if (!stored.isEmpty())
		{
			structureElement->linkEndChild(writeResources(stored, "storage"));
		}

		if (structure.isWarehouse())
		{
			structureElement->linkEndChild(NAS2D::dictionaryToAttributes(
				"warehouse_products",
				dynamic_cast<const Warehouse&>(structure).products().serialize()
			));
		}

		if (structure.isFoodStore())
		{
			structureElement->linkEndChild(
				NAS2D::dictionaryToAttributes(
					"food",
					{{{"level", dynamic_cast<const FoodProduction&>(structure).foodLevel()}}}
				)
			);
		}

		if (const auto* residence = dynamic_cast<const Residence*>(&structure))
		{
			structureElement->linkEndChild(
				NAS2D::dictionaryToAttributes(
					"waste",
					{{
						{"accumulated", residence->wasteAccumulated()},
						{"overflow", residence->wasteOverflow()},
					}}
				)
			);
		}

		if (structure.isMineFacility())
		{
			const auto& facility = dynamic_cast<const MineFacility&>(structure);

			structureElement->linkEndChild(
				NAS2D::dictionaryToAttributes(
					"trucks",
					{{{"assigned", facility.assignedTrucks()}}}
				)
			);
			structureElement->linkEndChild(
				NAS2D::dictionaryToAttributes(
					"extension",
					{{{"turns_remaining", facility.digTimeRemaining()}}}
				)
			);
		}

		if (structure.isMaintenance())
		{
			auto& maintenance = dynamic_cast<const MaintenanceFacility&>(structure);
			structureElement->linkEndChild(
				NAS2D::dictionaryToAttributes(
					"personnel",
					{{{"assigned", maintenance.personnel()}}}
				)
			);
		}

		return structureElement;
	}
}


StructureManager::StructureManager()
{
}


Structure& StructureManager::create(StructureID structureId, Tile& tile)
{
	auto& structure = *StructureCatalog::create(structureId, tile);
	addStructure(structure, tile);
	return structure;
}


void StructureManager::addStructure(Structure& structure, Tile& tile)
{
	if (std::find(mDeployedStructures.begin(), mDeployedStructures.end(), &structure) != mDeployedStructures.end())
	{
		throw std::runtime_error("StructureManager::addStructure(): Attempting to add a Structure that is already managed!");
	}

	if (tile.hasMapObject())
	{
		tile.removeMapObject();
	}

	mDeployedStructures.push_back(&structure);
	tile.mapObject(&structure);
}


void StructureManager::removeStructure(Structure& structure)
{
	const auto tileTableIt = std::find(mDeployedStructures.begin(), mDeployedStructures.end(), &structure);
	if (tileTableIt == mDeployedStructures.end())
	{
		throw std::runtime_error("StructureManager::removeStructure(): Attempting to remove a Structure that is not managed by the StructureManager.");
	}

	structure.tile().removeMapObject();
	mDeployedStructures.erase(tileTableIt);
	delete &structure;
}


void StructureManager::removeAllStructures()
{
	for (const auto* structure : mDeployedStructures)
	{
		structure->tile().removeMapObject();
		delete structure;
	}

	mDeployedStructures.clear();
}


StructureList StructureManager::structureList(StructureClass structureClass) const
{
	StructureList structuresOut;

	for (auto* structure : allStructures())
	{
		if (structure->structureClass() == structureClass)
		{
			structuresOut.push_back(structure);
		}
	}

	return structuresOut;
}


const StructureList& StructureManager::allStructures() const
{
	return mDeployedStructures;
}


StructureList StructureManager::agingStructures() const
{
	StructureList agingStructures{};

	for (auto* structure : mDeployedStructures)
	{
		if (structure->ages() && (structure->age() >= structure->maxAge() - 10))
		{
			agingStructures.push_back(structure);
		}
	}

	return agingStructures;
}


StructureList StructureManager::newlyBuiltStructures() const
{
	StructureList newlyBuiltStructures{};

	for (auto* structure : mDeployedStructures)
	{
		if (structure->ages() && structure->isNew())
		{
			newlyBuiltStructures.push_back(structure);
		}
	}

	return newlyBuiltStructures;
}


StructureList StructureManager::structuresWithCrime() const
{
	StructureList structuresWithCrime{};

	for (auto* structure : mDeployedStructures)
	{
		if (structure->hasCrime() && !structure->underConstruction())
		{
			structuresWithCrime.push_back(structure);
		}
	}

	return structuresWithCrime;
}


StructureList StructureManager::commandCenters() const
{
	StructureList structures;
	for (auto* structure : mDeployedStructures)
	{
		if (structure->isCommand())
		{
			structures.push_back(structure);
		}
	}
	return structures;
}


StructureList StructureManager::activeCommandCenters() const
{
	StructureList structures;
	for (auto* structure : mDeployedStructures)
	{
		if (structure->isCommand() && structure->operational())
		{
			structures.push_back(structure);
		}
	}
	return structures;
}


StructureList StructureManager::activePoliceStations() const
{
	StructureList structures;
	for (auto* structure : mDeployedStructures)
	{
		if (structure->isPolice() && structure->operational())
		{
			structures.push_back(structure);
		}
	}
	return structures;
}


bool StructureManager::hasCommandCenter() const
{
	for (auto* structure : mDeployedStructures)
	{
		if (structure->isCommand())
		{
			return true;
		}
	}
	return false;
}


CommandCenter& StructureManager::firstCc() const
{
	for (auto* structure : mDeployedStructures)
	{
		auto* commandCenter = dynamic_cast<CommandCenter*>(structure);
		if (commandCenter)
		{
			return *commandCenter;
		}
	}
	throw std::runtime_error("firstCc() called with no active CommandCenter");
}


std::vector<MapCoordinate> StructureManager::operationalCommandCenterPositions() const
{
	std::vector<MapCoordinate> positions;
	for (const auto* commandCenter : activeCommandCenters())
	{
		positions.push_back(commandCenter->xyz());
	}
	return positions;
}


bool StructureManager::isInCcRange(NAS2D::Point<int> position) const
{
	const auto range = StructureCatalog::getType(StructureID::CommandCenter).commRange;
	for (const auto* commandCenter : commandCenters())
	{
		const auto location = commandCenter->xyz().xy;
		if (isPointInRange(position, location, range))
		{
			return true;
		}
	}
	return false;
}


bool StructureManager::isInCommRange(NAS2D::Point<int> position) const
{
	for (const auto* structure : mDeployedStructures)
	{
		const auto commRange = structure->commRange();
		if (commRange > 0 && isPointInRange(position, structure->xyz().xy, commRange))
		{
			return true;
		}
	}

	return false;
}


void StructureManager::updateConnectedness(TileMap& tileMap)
{
	disconnectAll();
	walkGraph(operationalCommandCenterPositions(), tileMap);
}


std::vector<Tile*> StructureManager::getConnectednessOverlay() const
{
	std::vector<Tile*> result;
	result.reserve(mDeployedStructures.size());
	for (const auto* structure : mDeployedStructures)
	{
		if (structure->connected())
		{
			result.push_back(&structure->tile());
		}
	}
	return result;
}


void StructureManager::disconnectAll()
{
	for (auto* structure : mDeployedStructures)
	{
		structure->connected(false);
	}
}


int StructureManager::count() const
{
	return static_cast<int>(mDeployedStructures.size());
}


int StructureManager::operationalCount(StructureClass structureClass) const
{
	int count = 0;
	for (const auto* structure : mDeployedStructures)
	{
		if ((structure->structureClass() == structureClass) && structure->operational())
		{
			++count;
		}
	}
	return count;
}


int StructureManager::operationalCount(StructureID structureId) const
{
	int count = 0;
	for (const auto* structure : mDeployedStructures)
	{
		if ((structure->structureId() == structureId) && structure->operational())
		{
			++count;
		}
	}
	return count;
}


int StructureManager::disabledCount() const
{
	int count = 0;
	for (const auto* structure : mDeployedStructures)
	{
		if (structure->state() == StructureState::Disabled)
		{
			++count;
		}
	}
	return count;
}


int StructureManager::destroyedCount() const
{
	int count = 0;
	for (const auto* structure : mDeployedStructures)
	{
		if (structure->state() == StructureState::Destroyed)
		{
			++count;
		}
	}
	return count;
}


bool StructureManager::CHAPAvailable() const
{
	for (const auto* structure : mDeployedStructures)
	{
		if (structure->isChap() && structure->operational()) { return true; }
	}
	return false;
}


void StructureManager::updateEnergy()
{
	updateEnergyProduction();
	updateEnergyConsumed();
}


void StructureManager::updateEnergyProduction()
{
	mTotalEnergyOutput = 0;

	for (auto* structure : mDeployedStructures)
	{
		mTotalEnergyOutput += structure->energyProduced();
	}
}


void StructureManager::updateEnergyConsumed()
{
	mTotalEnergyUsed = 0;

	for (const auto* structure : mDeployedStructures)
	{
		if (structure->operational())
		{
			mTotalEnergyUsed += structure->energyRequirement();
		}
	}
}


int StructureManager::totalRefinedOreStorageCapacity() const
{
	int storageCapacity = 0;

	for (const auto* structure : mDeployedStructures)
	{
		if (structure->operational() || structure->isIdle())
		{
			storageCapacity += structure->refinedOreStorageCapacity();
		}
	}

	return storageCapacity;
}


int StructureManager::totalFoodStorageCapacity() const
{
	int storageCapacity = 0;

	for (const auto* structure : mDeployedStructures)
	{
		if (structure->operational() || structure->isIdle())
		{
			storageCapacity += structure->foodStorageCapacity();
		}
	}

	return storageCapacity;
}


int StructureManager::totalResidentialCapacity() const
{
	int residentialCapacity = 0;

	for (const auto* structure : mDeployedStructures)
	{
		if (structure->operational()) { residentialCapacity += structure->residentialCapacity(); }
	}

	return residentialCapacity;
}


int StructureManager::totalRobotCommandCapacity() const
{
	int totalRobotCommandCapacity = 0;

	for (const auto* structure : mDeployedStructures)
	{
		if (structure->operational())
		{
			totalRobotCommandCapacity += structure->type().robotCommandCapacity;
		}
	}

	return totalRobotCommandCapacity;
}


void StructureManager::assignColonistsToResidences(PopulationPool& population)
{
	int populationCount = population.size();
	for (auto* structure : mDeployedStructures)
	{
		Residence* residence = dynamic_cast<Residence*>(structure);
		if (residence && residence->operational())
		{
			residence->assignColonists(populationCount);
			populationCount -= residence->assignedColonists();
		}
	}
}


void StructureManager::assignScientistsToResearchFacilities(PopulationPool& population)
{
	int availableScientists = population.availableScientists();
	for (auto* laboratory : mDeployedStructures)
	{
		auto* lab = dynamic_cast<ResearchFacility*>(laboratory);
		if (lab)
		{
			lab->assignScientists(0);
			if (lab->operational())
			{
				lab->assignScientists(availableScientists);
				availableScientists -= lab->assignedScientists();
				population.usePopulation({0, lab->assignedScientists()});
			}
		}
	}
}


void StructureManager::update(const StorableResources& resources, PopulationPool& population)
{
	updateStructures(resources, population, mDeployedStructures);

	assignColonistsToResidences(population);

	/**
	 * Scientists are assigned to labs after other facilities like medical, university, etc
	 * as those are much higher priority than labs. If the player wants to free up scientists,
	 * they can either manually set a structure to idle or bulldoze it.
	 */
	assignScientistsToResearchFacilities(population);
}


NAS2D::Xml::XmlElement* StructureManager::serialize() const
{
	auto* structures = new NAS2D::Xml::XmlElement("structures");

	for (const auto* structure : mDeployedStructures)
	{
		structures->linkEndChild(serializeStructure(*structure));
	}

	return structures;
}


void StructureManager::updateStructures(const StorableResources& resources, PopulationPool& population, StructureList& structures)
{
	mTotalEnergyOutput = 0;
	mTotalEnergyUsed = 0;

	std::ranges::stable_sort(
		mDeployedStructures,
		std::ranges::greater(),
		[](const Structure* structure) { return structure->type().priority; }
	);

	for (auto* structure : structures)
	{
		updateStructure(resources, population, *structure);
	}
}


void StructureManager::updateStructure(const StorableResources& resources, PopulationPool& population, Structure& structure)
{
	structure.processTurn();

	// State Check
	// ASSUMPTION:	Construction sites are considered self sufficient until they are
	//				completed and connected to the rest of the colony.
	if (structure.underConstruction() || structure.destroyed())
	{
		return;
	}

	if (structure.disabled() && structure.disabledReason() == DisabledReason::StructuralIntegrity)
	{
		return;
	}

	// Connection Check
	if (!structure.connected() && !structure.selfSustained())
	{
		structure.disable(DisabledReason::Disconnected);
		return;
	}

	// CHAP Check
	if (structure.requiresCHAP() && !CHAPAvailable())
	{
		structure.disable(DisabledReason::Chap);
		return;
	}

	// Population Check
	const auto& populationRequired = structure.populationRequirements();
	auto& populationAvailable = structure.populationAvailable();

	populationAvailable = fillPopulationRequirements(population, populationRequired);

	if ((populationAvailable.workers < populationRequired.workers) ||
		(populationAvailable.scientists < populationRequired.scientists))
	{
		structure.disable(DisabledReason::Population);
		return;
	}

	if (structure.energyRequirement() > totalEnergyAvailable())
	{
		structure.disable(DisabledReason::Energy);
		return;
	}

	// Check that enough resources are available for input.
	if (!structure.isIdle() && !(resources >= structure.resourcesIn()))
	{
		structure.disable(DisabledReason::RefinedResources);
		return;
	}

	structure.enable();

	if (structure.operational())
	{
		population.usePopulation(populationRequired);

		auto consumed = structure.resourcesIn();
		removeRefinedResources(consumed);

		mTotalEnergyOutput += structure.energyProduced();
		mTotalEnergyUsed += structure.energyRequirement();

		structure.think();
	}
}
