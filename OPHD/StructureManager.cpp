// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "StructureManager.h"

#include "Constants.h"
#include "ProductPool.h"
#include "IOHelper.h"
#include "PopulationPool.h"
#include "Map/Tile.h"
#include "Things/Robots/Robot.h"
#include "Things/Structures/Structures.h"

#include <algorithm>
#include <sstream>

using namespace NAS2D::Xml;


/**
 * Fills population requirements fields in a Structure.
 */
static void fillPopulationRequirements(PopulationPool& _p, const PopulationRequirements* _populationRequired, PopulationRequirements* _populationAvailable)
{
	// Verbose but a lot easier to read than the ternary operators I was using before.

	// WORKERS
	if (_p.enoughPopulationAvailable(Population::PersonRole::ROLE_WORKER, (*_populationRequired)[0]))
	{
		(*_populationAvailable)[0] = (*_populationRequired)[0];
	}
	else
	{
		(*_populationAvailable)[0] = _p.populationAvailable(Population::PersonRole::ROLE_WORKER);
	}

	// SCIENTISTS
	if (_p.enoughPopulationAvailable(Population::PersonRole::ROLE_SCIENTIST, (*_populationRequired)[1]))
	{
		(*_populationAvailable)[1] = (*_populationRequired)[1];
	}
	else
	{
		(*_populationAvailable)[1] = _p.populationAvailable(Population::PersonRole::ROLE_SCIENTIST);
	}
}


/**
 *
 */
bool StructureManager::CHAPAvailable()
{
	for (auto chap : mStructureLists[Structure::StructureClass::LifeSupport])
	{
		if (chap->operational()) { return true; }
	}

	return false;
}


/**
 *
 */
void StructureManager::update(StorableResources& resources, PopulationPool& population)
{
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

	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Storage]); // Everything else.
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Park]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::SurfacePolice]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::UndergroundPolice]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::RecreationCenter]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Residence]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::RobotCommand]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Warehouse]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Laboratory]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Commercial]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::University]);
	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Communication]);

	updateStructures(resources, population, mStructureLists[Structure::StructureClass::Undefined]);

	updateFactoryProduction();
}


/**
 *
 */
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
 *
 */
void StructureManager::updateStructures(StorableResources& resources, PopulationPool& population, StructureList& structures)
{
	bool chapAvailable = CHAPAvailable();
	const PopulationRequirements* _populationRequired = nullptr;
	PopulationRequirements* _populationAvailable = nullptr;

	Structure* structure = nullptr;
	for (std::size_t i = 0; i < structures.size(); ++i)
	{
		structure = structures[i];
		structure->update();

		// State Check
		// ASSUMPTION:	Construction sites are considered self sufficient until they are
		//				completed and connected to the rest of the colony.
		if (structure->underConstruction() || structure->destroyed())
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
		if (structure->requiresCHAP() && !chapAvailable)
		{
			structure->disable(DisabledReason::Chap);
			continue;
		}


		// Population Check
		_populationRequired = &structure->populationRequirements();
		_populationAvailable = &structure->populationAvailable();

		fillPopulationRequirements(population, _populationRequired, _populationAvailable);

		if (!population.enoughPopulationAvailable(Population::PersonRole::ROLE_WORKER, (*_populationRequired)[0]) ||
			!population.enoughPopulationAvailable(Population::PersonRole::ROLE_SCIENTIST, (*_populationRequired)[1]))
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
			population.usePopulation(Population::PersonRole::ROLE_WORKER, (*_populationRequired)[0]);
			population.usePopulation(Population::PersonRole::ROLE_SCIENTIST, (*_populationRequired)[1]);

			resources = resources - structure->resourcesIn();
			mTotalEnergyUsed += structure->energyRequirement();

			structure->think();
		}
	}
}


/**
 * 
 */
void StructureManager::updateFactoryProduction()
{
	StructureList& structures = mStructureLists[Structure::StructureClass::Factory];

	for (std::size_t i = 0; i < structures.size(); ++i)
	{
		static_cast<Factory*>(structures[i])->updateProduction();
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
	tile->thingIsStructure(true);
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

	if (structures.empty())
	{
		throw std::runtime_error("StructureManager::removeStructure(): Attempting to remove a Structure that is not managed by the StructureManager.");
	}

	for (std::size_t i = 0; i < structures.size(); ++i)
	{
		if (structures[i] == structure)
		{
			structures.erase(structures.begin() + static_cast<std::ptrdiff_t>(i));
			break;
		}
	}

	auto tileTableIt = mStructureTileTable.find(structure);
	if (tileTableIt == mStructureTileTable.end())
	{
		throw std::runtime_error("StructureManager::removeStructure(): Attempting to remove a Structure that is not managed by the StructureManager.");
	}
	else
	{
		tileTableIt->second->deleteThing();
		mStructureTileTable.erase(tileTableIt);
	}
}


StructureList& StructureManager::structureList(Structure::StructureClass structureClass)
{
	return mStructureLists[structureClass];
}


/**
 * Resets the 'connected' flag on all structures in the primary structure list.
 */
void StructureManager::disconnectAll()
{
	for (auto st_it = mStructureTileTable.begin(); st_it != mStructureTileTable.end(); ++st_it)
	{
		st_it->second->connected(false);
	}
}


/**
 * Returns the number of structures currently being managed by the StructureManager.
 */
int StructureManager::count() const
{
	int count = 0;
	for (auto it = mStructureLists.begin(); it != mStructureLists.end(); ++it)
	{
		count += static_cast<int>(it->second.size());
	}

	return count;
}


/**
 *
 */
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
	for (auto it = mStructureLists.begin(); it != mStructureLists.end(); ++it)
	{
		count += getCountInState(it->first, StructureState::Disabled);
	}

	return count;
}


/**
 * Gets a count of the number of destroyed buildings.
 */
int StructureManager::destroyed()
{
	int count = 0;
	for (auto it = mStructureLists.begin(); it != mStructureLists.end(); ++it)
	{
		count += getCountInState(it->first, StructureState::Destroyed);
	}

	return count;
}


/**
 *
 */
void StructureManager::dropAllStructures()
{
	for (auto map_it = mStructureTileTable.begin(); map_it != mStructureTileTable.end(); ++map_it)
	{
		map_it->second->deleteThing();
	}

	mStructureTileTable.clear();
	mStructureLists.clear();
}


/**
 * 
 */
Tile& StructureManager::tileFromStructure(Structure* structure)
{
	auto it = mStructureTileTable.find(structure);
	if (it == mStructureTileTable.end())
	{
		throw std::runtime_error("Could not find tile for structure");
	}
	return *it->second;
}


/**
 * 
 */
void serializeStructure(NAS2D::Xml::XmlElement* _ti, Structure* structure, Tile* _t)
{
	const auto position = _t->position();
	_ti->attribute("x", position.x);
	_ti->attribute("y", position.y);
	_ti->attribute("depth", _t->depth());

	_ti->attribute("age", structure->age());
	_ti->attribute("state", static_cast<int>(structure->state()));
	_ti->attribute("forced_idle", structure->forceIdle());
	_ti->attribute("disabled_reason", static_cast<int>(structure->disabledReason()));
	_ti->attribute("idle_reason", static_cast<int>(structure->idleReason()));
	_ti->attribute("type", structure->structureId());
	_ti->attribute("direction", structure->connectorDirection());

	const auto& production = structure->production();
	if (production > StorableResources{ 0 })
	{
		writeResources(_ti, production, "production");
	}

	const auto& stored = structure->storage();
	if (stored > StorableResources{ 0 })
	{
		writeResources(_ti, stored, "storage");
	}

	_ti->attribute("pop0", structure->populationAvailable()[0]);
	_ti->attribute("pop1", structure->populationAvailable()[1]);
}


/**
 * 
 */
void StructureManager::serialize(NAS2D::Xml::XmlElement* element)
{
	auto* structures = new NAS2D::Xml::XmlElement("structures");

	for (auto it = mStructureTileTable.begin(); it != mStructureTileTable.end(); ++it)
	{
		auto* structure = new NAS2D::Xml::XmlElement("structure");
		serializeStructure(structure, it->first, it->second);

		if (it->first->isFactory())
		{
			structure->attribute("production_completed", static_cast<Factory*>(it->first)->productionTurnsCompleted());
			structure->attribute("production_type", static_cast<Factory*>(it->first)->productType());
		}

		if (it->first->isWarehouse())
		{
			auto* warehouse_products = new NAS2D::Xml::XmlElement("warehouse_products");
			static_cast<Warehouse*>(it->first)->products().serialize(warehouse_products);
			structure->linkEndChild(warehouse_products);
		}

		if (it->first->isRobotCommand())
		{
			auto* robots = new NAS2D::Xml::XmlElement("robots");

			const RobotList& rl = static_cast<RobotCommand*>(it->first)->robots();

			std::stringstream str;
			for (std::size_t i = 0; i < rl.size(); ++i)
			{
				str << rl[i]->id();
				if (i != rl.size() - 1) { str << ","; } // kind of a kludge
			}

			robots->attribute("robots", str.str());
			structure->linkEndChild(robots);
		}

		if (it->first->structureClass() == Structure::StructureClass::FoodProduction)
		{
			auto* food = new NAS2D::Xml::XmlElement("food");
			food->attribute("level", static_cast<FoodProduction*>(it->first)->foodLevel());
			structure->linkEndChild(food);
		}

		structures->linkEndChild(structure);
	}

	element->linkEndChild(structures);
}


bool StructureManager::structureConnected(Structure* structure)
{
	return mStructureTileTable[structure]->connected();
}
