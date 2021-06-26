#include "StructureManager.h"

#include "Constants.h"
#include "ProductPool.h"
#include "IOHelper.h"
#include "PopulationPool.h"
#include "Map/Tile.h"
#include "Things/Robots/Robot.h"
#include "Things/Structures/Structures.h"

#include "States/MapViewStateHelper.h" // <-- For removeRefinedResources()

#include <NAS2D/ParserHelper.h>

#include <algorithm>
#include <sstream>


namespace
{
	/**
	 * Fills population requirements fields in a Structure.
	 */
	void fillPopulationRequirements(PopulationPool& populationPool, const PopulationRequirements& required, PopulationRequirements& available)
	{
		available[0] = std::min(required[0], populationPool.populationAvailable(Population::PersonRole::ROLE_WORKER));
		available[1] = std::min(required[1], populationPool.populationAvailable(Population::PersonRole::ROLE_SCIENTIST));
	}


	NAS2D::Xml::XmlElement* serializeStructure(Structure* structure, Tile* tile)
	{
		const auto position = tile->position();
		NAS2D::Dictionary dictionary =
		{{
			{"x", position.x},
			{"y", position.y},
			{"depth", tile->depth()},
		}};
		dictionary += structure->getDataDict();

		auto* structureElement = dictionaryToAttributes("structure", dictionary);

		const auto& production = structure->production();
		if (!production.isEmpty())
		{
			writeResources(structureElement, production, "production");
		}

		const auto& stored = structure->storage();
		if (!stored.isEmpty())
		{
			writeResources(structureElement, stored, "storage");
		}

		if (structure->isWarehouse())
		{
			auto* warehouse_products = new NAS2D::Xml::XmlElement("warehouse_products");
			static_cast<Warehouse*>(structure)->products().serialize(warehouse_products);
			structureElement->linkEndChild(warehouse_products);
		}

		if (structure->isRobotCommand())
		{
			const auto& robots = static_cast<RobotCommand*>(structure)->robots();

			std::stringstream str;
			for (std::size_t i = 0; i < robots.size(); ++i)
			{
				str << robots[i]->id();
				if (i != robots.size() - 1) { str << ","; } // kind of a kludge
			}

			structureElement->linkEndChild(
				NAS2D::dictionaryToAttributes("robots", {{{"robots", str.str()}}})
			);
		}

		if (structure->structureClass() == Structure::StructureClass::FoodProduction ||
			structure->structureId() == StructureID::SID_COMMAND_CENTER)
		{
			structureElement->linkEndChild(
				NAS2D::dictionaryToAttributes(
					"food",
					{{{"level", static_cast<FoodProduction*>(structure)->foodLevel()}}}
				)
			);
		}

		if (structure->structureClass() == Structure::StructureClass::Residence)
		{
			Residence* residence = static_cast<Residence*>(structure);
			auto* waste = new NAS2D::Xml::XmlElement("waste");
			waste->attribute("accumulated", residence->wasteAccumulated());
			waste->attribute("overflow", residence->wasteOverflow());
			structureElement->linkEndChild(waste);
		}

		if (structure->isMineFacility())
		{
			MineFacility* facility = static_cast<MineFacility*>(structure);

			auto* trucks = new NAS2D::Xml::XmlElement("trucks");
			trucks->attribute("assigned", facility->assignedTrucks());

			auto* extension = new NAS2D::Xml::XmlElement("extension");
			extension->attribute("turns_remaining", facility->digTimeRemaining());

			structureElement->linkEndChild(trucks);
			structureElement->linkEndChild(extension);
		}

		if (structure->structureClass() == Structure::StructureClass::Maintenance)
		{
			auto maintenance = static_cast<MaintenanceFacility*>(structure);
			auto personnel = new NAS2D::Xml::XmlElement("personnel");
			personnel->attribute("assigned", maintenance->personnel());
			structureElement->linkEndChild(personnel);
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
			if (structure->operational() || structure->isIdle())
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

		fillPopulationRequirements(population, populationRequired, populationAvailable);

		if ((populationAvailable[0] < populationRequired[0]) ||
			(populationAvailable[1] < populationRequired[1]))
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

		if (structure->operational() || structure->isIdle())
		{
			population.usePopulation(Population::PersonRole::ROLE_WORKER, populationRequired[0]);
			population.usePopulation(Population::PersonRole::ROLE_SCIENTIST, populationRequired[1]);

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
void StructureManager::addStructure(Structure* structure, Tile* tile)
{
	// Sanity checks
	if (tile == nullptr)
	{
		return;
	}

	if (mStructureTileTable.find(structure) != mStructureTileTable.end())
	{
		throw std::runtime_error("StructureManager::addStructure(): Attempting to add a Structure that is already managed!");
	}

	// Remove things from tile only if we know we're adding a structure.
	if (!tile->empty())
	{
		tile->removeThing();
	}

	mStructureTileTable[structure] = tile;

	mStructureLists[structure->structureClass()].push_back(structure);
	tile->pushThing(structure);
}


/**
 * Removes a Structure from the StructureManager.
 *
 * \warning	A Structure removed from the StructureManager will be freed.
 *			Remaining pointers and references will be invalidated.
 */
void StructureManager::removeStructure(Structure* structure)
{
	StructureList& structures = mStructureLists[structure->structureClass()];

	const auto it = std::find(structures.begin(), structures.end(), structure);
	const auto isFoundStructureTable = it != structures.end();
	if (isFoundStructureTable)
	{
		structures.erase(it);
	}

	const auto tileTableIt = mStructureTileTable.find(structure);
	const auto isFoundTileTable = tileTableIt != mStructureTileTable.end();
	if (isFoundTileTable)
	{
		tileTableIt->second->deleteThing();
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

	for(auto& classListPair : mStructureLists)
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
		pair.second->deleteThing();
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


void StructureManager::serialize(NAS2D::Xml::XmlElement* element)
{
	auto* structures = new NAS2D::Xml::XmlElement("structures");

	for (auto& [structure, tile] : mStructureTileTable)
	{
		structures->linkEndChild(serializeStructure(structure, tile));
	}

	element->linkEndChild(structures);
}


bool StructureManager::structureConnected(Structure* structure)
{
	return mStructureTileTable[structure]->connected();
}
