#include "StructureManager.h"

#include "Constants/Numbers.h"
#include "ProductPool.h"
#include "IOHelper.h"
#include "PopulationPool.h"
#include "Map/Tile.h"
#include "MapObjects/Robots/Robot.h"

#include "States/MapViewStateHelper.h" // <-- For removeRefinedResources()

#include <NAS2D/ParserHelper.h>
#include <NAS2D/StringUtils.h>
#include <NAS2D/ContainerUtils.h>

#include <algorithm>
#include <sstream>


namespace
{
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


	NAS2D::Xml::XmlElement* serializeStructure(Structure& structure, Tile& tile)
	{
		const auto position = tile.xyz();
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
				static_cast<Warehouse&>(structure).products().serialize()
			));
		}

		if (structure.isFoodStore())
		{
			structureElement->linkEndChild(
				NAS2D::dictionaryToAttributes(
					"food",
					{{{"level", static_cast<FoodProduction&>(structure).foodLevel()}}}
				)
			);
		}

		if (structure.structureClass() == Structure::StructureClass::Residence)
		{
			Residence& residence = static_cast<Residence&>(structure);
			structureElement->linkEndChild(
				NAS2D::dictionaryToAttributes(
					"waste",
					{{
						{"accumulated", residence.wasteAccumulated()},
						{"overflow", residence.wasteOverflow()},
					}}
				)
			);
		}

		if (structure.isMineFacility())
		{
			MineFacility& facility = static_cast<MineFacility&>(structure);

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

		if (structure.structureClass() == Structure::StructureClass::Maintenance)
		{
			auto& maintenance = static_cast<MaintenanceFacility&>(structure);
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


bool StructureManager::CHAPAvailable()
{
	for (auto chap : mStructureLists[Structure::StructureClass::LifeSupport])
	{
		if (chap->operational()) { return true; }
	}

	return false;
}


void StructureManager::update(const StorableResources& resources, PopulationPool& population)
{
	mAgingStructures.clear();
	mNewlyBuiltStructures.clear();
	mStructuresWithCrime.clear();

	// Called separately so that 1) high priority structures can be updated first and
	// 2) so that resource handling code (like energy) can be handled between update
	// calls to lower priority structures.
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Lander]); // No resource needs
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Command]); // Self sufficient
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::EnergyProduction]); // Nothing can work without energy

	updateEnergyProduction();

	// Basic resource production
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Mine]); // Can't operate without resources.
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Smelter]);

	updateStructures(resources, population, mStructureLists[Structure::StructureClass::LifeSupport]); // Air, water food must come before others
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::FoodProduction]);

	updateStructures(resources, population, mStructureLists[Structure::StructureClass::MedicalCenter]); // No medical facilities, people die
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Nursery]);

	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Factory]); // Production
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Maintenance]);

	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Storage]); // Everything else.
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Park]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::SurfacePolice]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::UndergroundPolice]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::RecreationCenter]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Recycling]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Residence]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::RobotCommand]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Warehouse]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Laboratory]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Commercial]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::University]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Communication]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Road]);

	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Undefined]);

	assignColonistsToResidences(population);
	
	/**
	 * Scientists are assigned to labs after other facilities like medical, university, etc
	 * as those are much higher priority than labs. If the player wants to free up scientists,
	 * they can either manually set a structure to idle or bulldoze it.
	 */
	assignScientistsToResearchFacilities(population);
}


void StructureManager::updateEnergyProduction()
{
	mTotalEnergyOutput = 0;
	mTotalEnergyUsed = 0;

	for (auto structure : mStructureLists[Structure::StructureClass::EnergyProduction])
	{
		auto powerStructure = static_cast<PowerStructure*>(structure);
		if (powerStructure->operational())
		{
			mTotalEnergyOutput += powerStructure->energyProduced();
		}
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
		for (auto structure : classListPair.second)
		{
			if (structure->operational())
			{
				mTotalEnergyUsed += structure->energyRequirement();
			}
		}
	}
}


void StructureManager::assignColonistsToResidences(PopulationPool& population)
{
	int populationCount = population.size();
	for (auto structure : mStructureLists[Structure::StructureClass::Residence])
	{
		Residence* residence = static_cast<Residence*>(structure);
		if (residence->operational())
		{
			residence->assignColonists(populationCount);
			populationCount -= residence->assignedColonists();
		}
	}
}


void StructureManager::assignScientistsToResearchFacilities(PopulationPool& population)
{
	int availableScientists = population.availableScientists();
	for (auto laboratory : mStructureLists[Structure::StructureClass::Laboratory])
	{
		ResearchFacility* lab = static_cast<ResearchFacility*>(laboratory);
		lab->assignScientsts(0);
		if (lab->operational())
		{
			lab->assignScientsts(availableScientists);
			availableScientists -= lab->assignedScientists();
			population.usePopulation({0, lab->assignedScientists()});
		}
	}
}


void StructureManager::updateStructures(const StorableResources& resources, PopulationPool& population, StructureList& structures)
{
	Structure* structure = nullptr;
	for (std::size_t i = 0; i < structures.size(); ++i)
	{
		structure = structures[i];
		structure->update();

		if (structure->ages() && (structure->age() >= structure->maxAge() - 10))
		{
			mAgingStructures.push_back(structure);
		}

		if (structure->age() == structure->turnsToBuild())
		{
			mNewlyBuiltStructures.push_back(structure);
		}

		if (structure->hasCrime() && !structure->underConstruction())
		{
			mStructuresWithCrime.push_back(structure);
		}

		// State Check
		// ASSUMPTION:	Construction sites are considered self sufficient until they are
		//				completed and connected to the rest of the colony.
		if (structure->underConstruction() || structure->destroyed())
		{
			continue;
		}

		if (structure->disabled() && structure->disabledReason() == DisabledReason::StructuralIntegrity)
		{
			continue;
		}

		// Connection Check
		if (!structureConnected(structure) && !structure->selfSustained())
		{
			structure->disable(DisabledReason::Disconnected);
			continue;
		}

		// CHAP Check
		if (structure->requiresCHAP() && !CHAPAvailable())
		{
			structure->disable(DisabledReason::Chap);
			continue;
		}

		// Population Check
		const auto& populationRequired = structure->populationRequirements();
		auto& populationAvailable = structure->populationAvailable();

		populationAvailable = fillPopulationRequirements(population, populationRequired);

		if ((populationAvailable.workers < populationRequired.workers) ||
			(populationAvailable.scientists < populationRequired.scientists))
		{
			structure->disable(DisabledReason::Population);
			continue;
		}

		if (structure->energyRequirement() > totalEnergyAvailable())
		{
			structure->disable(DisabledReason::Energy);
			continue;
		}

		// Check that enough resources are available for input.
		if (!structure->isIdle() && !(resources >= structure->resourcesIn()))
		{
			structure->disable(DisabledReason::RefinedResources);
			continue;
		}

		structure->enable();

		if (structure->operational())
		{
			population.usePopulation(populationRequired);

			auto consumed = structure->resourcesIn();
			removeRefinedResources(consumed);

			mTotalEnergyUsed += structure->energyRequirement();

			structure->think();
		}
	}
}


/**
 * Adds a new Structure to the StructureManager.
 */
void StructureManager::addStructure(Structure& structure, Tile& tile)
{
	if (mStructureTileTable.find(&structure) != mStructureTileTable.end())
	{
		throw std::runtime_error("StructureManager::addStructure(): Attempting to add a Structure that is already managed!");
	}

	// Remove things from tile only if we know we're adding a structure.
	if (!tile.empty())
	{
		tile.removeMapObject();
	}

	mStructureTileTable[&structure] = &tile;

	mStructureLists[structure.structureClass()].push_back(&structure);
	tile.pushMapObject(&structure);
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
	if (isFoundStructureTable)
	{
		structures.erase(it);
	}

	const auto tileTableIt = mStructureTileTable.find(&structure);
	const auto isFoundTileTable = tileTableIt != mStructureTileTable.end();
	if (isFoundTileTable)
	{
		tileTableIt->second->deleteMapObject();
		mStructureTileTable.erase(tileTableIt);
	}

	if (!isFoundStructureTable || !isFoundTileTable)
	{
		throw std::runtime_error("StructureManager::removeStructure(): Attempting to remove a Structure that is not managed by the StructureManager.");
	}
}


const StructureList& StructureManager::structureList(Structure::StructureClass structureClass)
{
	return mStructureLists[structureClass];
}


StructureList StructureManager::allStructures()
{
	StructureList structuresOut;

	for (auto& classListPair : mStructureLists)
	{
		auto& structures = classListPair.second;
		std::copy(structures.begin(), structures.end(), std::back_inserter(structuresOut));
	}

	return structuresOut;
}


/**
 * Resets the 'connected' flag on all structures in the primary structure list.
 */
void StructureManager::disconnectAll()
{
	for (auto& pair : mStructureTileTable)
	{
		pair.second->connected(false);
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


int StructureManager::getCountInState(Structure::StructureClass structureClass, StructureState state)
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


/**
 * Gets a count of the number of disabled buildings.
 */
int StructureManager::disabled()
{
	int count = 0;
	for (auto& pair : mStructureLists)
	{
		count += getCountInState(pair.first, StructureState::Disabled);
	}

	return count;
}


/**
 * Gets a count of the number of destroyed buildings.
 */
int StructureManager::destroyed()
{
	int count = 0;
	for (auto& pair : mStructureLists)
	{
		count += getCountInState(pair.first, StructureState::Destroyed);
	}

	return count;
}


void StructureManager::dropAllStructures()
{
	for (auto& pair : mStructureTileTable)
	{
		pair.second->deleteMapObject();
	}

	mStructureTileTable.clear();
	mStructureLists.clear();
}


Tile& StructureManager::tileFromStructure(Structure* structure)
{
	auto it = mStructureTileTable.find(structure);
	if (it == mStructureTileTable.end())
	{
		throw std::runtime_error("Could not find tile for structure");
	}
	return *it->second;
}


NAS2D::Xml::XmlElement* StructureManager::serialize()
{
	auto* structures = new NAS2D::Xml::XmlElement("structures");

	for (auto& [structure, tile] : mStructureTileTable)
	{
		structures->linkEndChild(serializeStructure(*structure, *tile));
	}

	return structures;
}


bool StructureManager::structureConnected(Structure* structure)
{
	return mStructureTileTable[structure]->connected();
}
