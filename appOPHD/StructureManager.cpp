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
	auto populateKeys()
	{
		std::map<StructureClass, StructureList> result;
		for (const auto structureClass : allStructureClasses())
		{
			result[structureClass]; // Generate blank array value for given key
		}
		return result;
	}


	/**
	 * Fills population requirements fields in a Structure.
	 */
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

		if (structure.structureClass() == StructureClass::Maintenance)
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


StructureManager::StructureManager() :
	mStructureLists{populateKeys()}
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

	mStructureLists[structure.structureClass()].push_back(&structure);
	tile.mapObject(&structure);
}


/**
 * Removes a Structure from the StructureManager.
 *
 * \warning	A Structure removed from the StructureManager will be freed.
 *			Remaining pointers and references will be invalidated.
 */
void StructureManager::removeStructure(Structure& structure)
{
	StructureList& structures = mStructureLists[structure.structureClass()];

	const auto it = std::find(structures.begin(), structures.end(), &structure);
	const auto isFoundStructureTable = it != structures.end();

	const auto tileTableIt = std::find(mDeployedStructures.begin(), mDeployedStructures.end(), &structure);
	const auto isFoundTileTable = tileTableIt != mDeployedStructures.end();

	if (!isFoundStructureTable || !isFoundTileTable)
	{
		throw std::runtime_error("StructureManager::removeStructure(): Attempting to remove a Structure that is not managed by the StructureManager.");
	}

	structures.erase(it);
	(*tileTableIt)->tile().removeMapObject();
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
	mStructureLists = populateKeys();
}


const StructureList& StructureManager::structureList(StructureClass structureClass) const
{
	return mStructureLists.at(structureClass);
}


StructureList StructureManager::allStructures() const
{
	StructureList structuresOut;

	for (auto& classListPair : mStructureLists)
	{
		auto& structures = classListPair.second;
		std::copy(structures.begin(), structures.end(), std::back_inserter(structuresOut));
	}

	return structuresOut;
}


StructureList StructureManager::agingStructures() const
{
	StructureList agingStructures{};

	for (auto* structure : allStructures())
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
	return mNewlyBuiltStructures;
}


StructureList StructureManager::structuresWithCrime() const
{
	return mStructuresWithCrime;
}


StructureList StructureManager::activePoliceStations() const
{
	StructureList policeStations;
	for (auto* structure : allStructures())
	{
		if (structure->operational() && structure->isPolice())
		{
			policeStations.push_back(structure);
		}
	}
	return policeStations;
}


bool StructureManager::hasCommandCenter() const
{
	return !getStructures<CommandCenter>().empty();
}


CommandCenter& StructureManager::firstCc() const
{
	const auto& ccList = getStructures<CommandCenter>();
	if (ccList.empty())
	{
		throw std::runtime_error("firstCc() called with no active CommandCenter");
	}
	return *ccList.at(0);
}


std::vector<MapCoordinate> StructureManager::operationalCommandCenterPositions() const
{
	std::vector<MapCoordinate> positions;
	for (const auto* commandCenter : structureList(StructureClass::Command))
	{
		if (commandCenter->operational())
		{
			positions.push_back(commandCenter->xyz());
		}
	}
	return positions;
}


bool StructureManager::isInCcRange(NAS2D::Point<int> position) const
{
	const auto range = StructureCatalog::getType(StructureID::CommandCenter).commRange;
	const auto& ccList = getStructures<CommandCenter>();
	for (const auto* commandCenter : ccList)
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
	const auto& structures = allStructures();
	for (const auto* structure : structures)
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


/**
 * Resets the 'connected' flag on all structures in the primary structure list.
 */
void StructureManager::disconnectAll()
{
	for (auto* structure : mDeployedStructures)
	{
		structure->connected(false);
	}
}


/**
 * Returns the number of structures currently being managed by the StructureManager.
 */
int StructureManager::count() const
{
	int count = 0;
	for (auto& pair : mStructureLists)
	{
		count += static_cast<int>(pair.second.size());
	}

	return count;
}


int StructureManager::getCountInState(StructureClass structureClass, StructureState state) const
{
	int count = 0;
	for (const auto* structure : structureList(structureClass))
	{
		if (structure->state() == state)
		{
			++count;
		}
	}
	return count;
}


int StructureManager::disabledCount() const
{
	int count = 0;
	for (auto& pair : mStructureLists)
	{
		count += getCountInState(pair.first, StructureState::Disabled);
	}

	return count;
}


int StructureManager::destroyedCount() const
{
	int count = 0;
	for (auto& pair : mStructureLists)
	{
		count += getCountInState(pair.first, StructureState::Destroyed);
	}

	return count;
}


bool StructureManager::CHAPAvailable() const
{
	for (const auto* chap : structureList(StructureClass::LifeSupport))
	{
		if (chap->operational()) { return true; }
	}

	return false;
}


void StructureManager::updateEnergyProduction()
{
	mTotalEnergyOutput = 0;
	mTotalEnergyUsed = 0;

	for (auto* structure : allStructures())
	{
		mTotalEnergyOutput += structure->energyProduced();
	}
}


/**
 * Updates the consumed energy used cache value. Does not
 * modify any structure values.
 */
void StructureManager::updateEnergyConsumed()
{
	mTotalEnergyUsed = 0;

	for (auto& classListPair : mStructureLists)
	{
		for (const auto* structure : classListPair.second)
		{
			if (structure->operational())
			{
				mTotalEnergyUsed += structure->energyRequirement();
			}
		}
	}
}


int StructureManager::totalRefinedOreStorageCapacity() const
{
	int storageCapacity = 0;

	for (const auto* structure : allStructures())
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

	for (const auto* structure : allStructures())
	{
		if (structure->operational() || structure->isIdle())
		{
			storageCapacity += structure->foodStorageCapacity();
		}
	}

	return storageCapacity;
}


int StructureManager::totalRobotCommandCapacity() const
{
	int totalRobotCommandCapacity = 0;

	for (const auto* structure : allStructures())
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
	for (auto* structure : mStructureLists[StructureClass::Residence])
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
	for (auto* laboratory : mStructureLists[StructureClass::Laboratory])
	{
		auto* lab = dynamic_cast<ResearchFacility*>(laboratory);
		lab->assignScientists(0);
		if (lab->operational())
		{
			lab->assignScientists(availableScientists);
			availableScientists -= lab->assignedScientists();
			population.usePopulation({0, lab->assignedScientists()});
		}
	}
}


void StructureManager::update(const StorableResources& resources, PopulationPool& population)
{
	mNewlyBuiltStructures.clear();
	mStructuresWithCrime.clear();

	// Called separately so that 1) high priority structures can be updated first and
	// 2) so that resource handling code (like energy) can be handled between update
	// calls to lower priority structures.
	updateStructures(resources, population, mStructureLists[StructureClass::Lander]); // No resource needs
	updateStructures(resources, population, mStructureLists[StructureClass::Command]); // Self sufficient
	updateStructures(resources, population, mStructureLists[StructureClass::EnergyProduction]); // Nothing can work without energy

	updateEnergyProduction();

	// Basic resource production
	updateStructures(resources, population, mStructureLists[StructureClass::Mine]); // Can't operate without resources.
	updateStructures(resources, population, mStructureLists[StructureClass::Smelter]);

	updateStructures(resources, population, mStructureLists[StructureClass::LifeSupport]); // Air, water food must come before others
	updateStructures(resources, population, mStructureLists[StructureClass::FoodProduction]);

	updateStructures(resources, population, mStructureLists[StructureClass::MedicalCenter]); // No medical facilities, people die
	updateStructures(resources, population, mStructureLists[StructureClass::Nursery]);

	updateStructures(resources, population, mStructureLists[StructureClass::Factory]); // Production
	updateStructures(resources, population, mStructureLists[StructureClass::Maintenance]);

	updateStructures(resources, population, mStructureLists[StructureClass::Storage]); // Everything else.
	updateStructures(resources, population, mStructureLists[StructureClass::Park]);
	updateStructures(resources, population, mStructureLists[StructureClass::SurfacePolice]);
	updateStructures(resources, population, mStructureLists[StructureClass::UndergroundPolice]);
	updateStructures(resources, population, mStructureLists[StructureClass::RecreationCenter]);
	updateStructures(resources, population, mStructureLists[StructureClass::Recycling]);
	updateStructures(resources, population, mStructureLists[StructureClass::Residence]);
	updateStructures(resources, population, mStructureLists[StructureClass::RobotCommand]);
	updateStructures(resources, population, mStructureLists[StructureClass::Warehouse]);
	updateStructures(resources, population, mStructureLists[StructureClass::Laboratory]);
	updateStructures(resources, population, mStructureLists[StructureClass::Commercial]);
	updateStructures(resources, population, mStructureLists[StructureClass::University]);
	updateStructures(resources, population, mStructureLists[StructureClass::Communication]);
	updateStructures(resources, population, mStructureLists[StructureClass::Road]);

	updateStructures(resources, population, mStructureLists[StructureClass::Undefined]);

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
	for (auto* structure : structures)
	{
		updateStructure(resources, population, *structure);
	}
}


void StructureManager::updateStructure(const StorableResources& resources, PopulationPool& population, Structure& structure)
{
	structure.processTurn();

	if (structure.age() == structure.turnsToBuild())
	{
		mNewlyBuiltStructures.push_back(&structure);
	}

	if (structure.hasCrime() && !structure.underConstruction())
	{
		mStructuresWithCrime.push_back(&structure);
	}

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

		mTotalEnergyUsed += structure.energyRequirement();

		structure.think();
	}
}
