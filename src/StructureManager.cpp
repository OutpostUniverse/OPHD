// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "StructureManager.h"

#include "Constants.h"
#include "ProductPool.h"
#include "StructureTranslator.h"

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
	if (_p.enoughPopulationAvailable(Population::ROLE_WORKER, (*_populationRequired)[0]))
	{
		(*_populationAvailable)[0] = (*_populationRequired)[0];
	}
	else
	{
		(*_populationAvailable)[0] = _p.populationAvailable(Population::ROLE_WORKER);
	}

	// SCIENTISTS
	if (_p.enoughPopulationAvailable(Population::ROLE_SCIENTIST, (*_populationRequired)[1]))
	{
		(*_populationAvailable)[1] = (*_populationRequired)[1];
	}
	else
	{
		(*_populationAvailable)[1] = _p.populationAvailable(Population::ROLE_SCIENTIST);
	}
}


/**
 *
 */
bool StructureManager::CHAPAvailable()
{
	for (auto chap : mStructureLists[Structure::CLASS_LIFE_SUPPORT])
	{
		if (chap->operational()) { return true; }
	}

	return false;
}


/**
 *
 */
void StructureManager::update(ResourcePool& _r, PopulationPool& _p)
{
	// Called separately so that 1) high priority structures can be updated first and
	// 2) so that resource handling code (like energy) can be handled between update
	// calls to lower priority structures.
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_LANDER]);				// No resource needs
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_COMMAND]);			// Self sufficient
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_ENERGY_PRODUCTION]);	// Nothing can work without energy

	updateEnergyProduction(_r, _p);

	// Basic resource production
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_MINE]);				// Can't operate without resources.
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_SMELTER]);

	updateStructures(_r, _p, mStructureLists[Structure::CLASS_LIFE_SUPPORT]);		// Air, water food must come before others
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_FOOD_PRODUCTION]);

	updateStructures(_r, _p, mStructureLists[Structure::CLASS_MEDICAL_CENTER]);		// No medical facilities, people die
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_NURSERY]);

	updateStructures(_r, _p, mStructureLists[Structure::CLASS_FACTORY]);			// Production

	updateStructures(_r, _p, mStructureLists[Structure::CLASS_STORAGE]);			// Everything else.
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_PARK]);
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_SURFACE_POLICE]);
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_UNDERGROUND_POLICE]);
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_RECREATION_CENTER]);
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_RESIDENCE]);
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_ROBOT_COMMAND]);
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_WAREHOUSE]);
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_LABORATORY]);
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_COMMERCIAL]);
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_UNIVERSITY]);
	updateStructures(_r, _p, mStructureLists[Structure::CLASS_COMM]);

	updateStructures(_r, _p, mStructureLists[Structure::CLASS_UNDEFINED]);

	updateFactoryProduction();
}


/**
 *
 */
void StructureManager::updateEnergyProduction(ResourcePool& _r, PopulationPool& _p)
{
	mTotalEnergyOutput = 0;

	for (auto _s : mStructureLists[Structure::CLASS_ENERGY_PRODUCTION])
	{
		if (_s->operational()) { mTotalEnergyOutput += _s->resourcesOut().energy(); }
	}

	_r.energy(mTotalEnergyOutput);
}


/**
 *
 */
void StructureManager::updateStructures(ResourcePool& _r, PopulationPool& _p, StructureList& _sl)
{
	bool chapAvailable = CHAPAvailable();
	const PopulationRequirements* _populationRequired = nullptr;
	PopulationRequirements* _populationAvailable = nullptr;

	Structure* structure = nullptr;
	for (size_t i = 0; i < _sl.size(); ++i)
	{
		structure = _sl[i];
		structure->update();

		// State Check
		// ASSUMPTION:	Construction sites are considered self sufficient until they are
		//				completed and connected to the rest of the colony.
		if (structure->underConstruction() || structure->destroyed())
			continue;

		// Connection Check
		if (!structureConnected(structure) && !structure->selfSustained())
		{
			structure->disable(DISABLED_DISCONNECTED);
			continue;
		}

		// CHAP Check
		if (structure->requiresCHAP() && !chapAvailable)
		{
			structure->disable(DISABLED_CHAP);
			continue;
		}


		// Population Check
		_populationRequired = &structure->populationRequirements();
		_populationAvailable = &structure->populationAvailable();

		fillPopulationRequirements(_p, _populationRequired, _populationAvailable);

		if (!_p.enoughPopulationAvailable(Population::ROLE_WORKER, (*_populationRequired)[0]) ||
			!_p.enoughPopulationAvailable(Population::ROLE_SCIENTIST, (*_populationRequired)[1]))
		{
			structure->disable(DISABLED_POPULATION);
			continue;
		}
		else
		{
			structure->enable();
		}

		// Check that enough resources are available for input.
		if (!structure->resourcesIn().empty() && !structure->enoughResourcesAvailable(_r) && !structure->isIdle())
		{
			if (!structure->isIdle()) //-V571
			{
				/// \fixme	Ugly. Special case code specifically to determine if energy is the reason for a disabled structure.
				if (structure->resourcesIn().energy() > _r.energy()) { structure->disable(DISABLED_ENERGY); }
				else { structure->disable(DISABLED_REFINED_RESOURCES); }
				continue;
			}
		}
		else
		{
			structure->enable();
		}


		if (structure->operational() || structure->isIdle())
		{
			_p.usePopulation(Population::ROLE_WORKER, (*_populationRequired)[0]);
			_p.usePopulation(Population::ROLE_SCIENTIST, (*_populationRequired)[1]);

			_r -= structure->resourcesIn();

			structure->think();
		}

		// handle output resources
	}
}


/**
 * 
 */
void StructureManager::updateFactoryProduction()
{
	StructureList& sl = mStructureLists[Structure::CLASS_FACTORY];

	for (size_t i = 0; i < sl.size(); ++i)
	{
		static_cast<Factory*>(sl[i])->updateProduction();
	}
}


/**
 * Adds a new Structure to the StructureManager.
 */
void StructureManager::addStructure(Structure* st, Tile* t)
{
	// Sanity checks
	if (t == nullptr)
	{
		return;
	}

	if (mStructureTileTable.find(st) != mStructureTileTable.end())
	{
		throw std::runtime_error("StructureManager::addStructure(): Attempting to add a Structure that is already managed!");
		return;
	}

	// Remove thing's from tile only if we know we're adding a structure.
	if (!t->empty())
	{
		t->removeThing();
	}

	mStructureTileTable[st] = t;

	mStructureLists[st->structureClass()].push_back(st);
	t->pushThing(st);
	t->thingIsStructure(true);
}


/**
 * Removes a Structure from the StructureManager.
 *
 * \warning	A Structure removed from the StructureManager will be freed.
 *			Remaining pointers and references will be invalidated.
 */
void StructureManager::removeStructure(Structure* st)
{
	StructureList& sl = mStructureLists[st->structureClass()];

	if (sl.empty())
	{
		throw std::runtime_error("StructureManager::removeStructure(): Attempting to remove a Structure that is not managed by the StructureManager.");
	}

	for (size_t i = 0; i < sl.size(); ++i)
	{
		if (sl[i] == st)
		{
			sl.erase(sl.begin() + i);
			break;
		}
	}

	auto tileTableIt = mStructureTileTable.find(st);
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
		count += it->second.size();
	}

	return count;
}


/**
 *
 */
int StructureManager::getCountInState(Structure::StructureClass _st, Structure::StructureState _state)
{
	int count = 0;
	for (size_t i = 0; i < structureList(_st).size(); ++i)
	{
		if (structureList(_st)[i]->state() == _state)
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
		count += getCountInState(it->first, Structure::DISABLED);
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
		count += getCountInState(it->first, Structure::DESTROYED);
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
Tile* StructureManager::tileFromStructure(Structure* _st)
{
	auto it = mStructureTileTable.find(_st);
	if (it != mStructureTileTable.end()) { return it->second; }
	return nullptr;
}


/**
 * 
 */
void serializeResourcePool(XmlElement* _ti, ResourcePool& _rp, const std::string& name)
{
	XmlElement* pool = new XmlElement(name);
	_rp.serialize(pool);
	_ti->linkEndChild(pool);
}


/**
 *
 */
void serializeStructure(XmlElement* _ti, Structure* _s, Tile* _t)
{
	_ti->attribute("x", _t->x());
	_ti->attribute("y", _t->y());
	_ti->attribute("depth", _t->depth());

	_ti->attribute("age", _s->age());
	_ti->attribute("state", _s->state());
	_ti->attribute("forced_idle", _s->forceIdle());
	_ti->attribute("disabled_reason", static_cast<int>(_s->disabledReason()));
	_ti->attribute("idle_reason", static_cast<int>(_s->idleReason()));
	_ti->attribute("type", _s->name());
	_ti->attribute("direction", _s->connectorDirection());

	if (!_s->production().empty())
	{
		serializeResourcePool(_ti, _s->production(), "production");
	}

	if (!_s->storage().empty())
	{
		serializeResourcePool(_ti, _s->storage(), "storage");
	}

	_ti->attribute("pop0", _s->populationAvailable()[0]);
	_ti->attribute("pop1", _s->populationAvailable()[1]);
}


/**
 * 
 */
void StructureManager::serialize(XmlElement* _ti)
{
	XmlElement* structures = new XmlElement("structures");

	for (auto it = mStructureTileTable.begin(); it != mStructureTileTable.end(); ++it)
	{
		XmlElement* structure = new XmlElement("structure");
		serializeStructure(structure, it->first, it->second);

		if (it->first->isFactory())
		{
			structure->attribute("production_completed", static_cast<Factory*>(it->first)->productionTurnsCompleted());
			structure->attribute("production_type", static_cast<Factory*>(it->first)->productType());
		}

		if (it->first->isWarehouse())
		{
			XmlElement* warehouse_products = new XmlElement("warehouse_products");
			static_cast<Warehouse*>(it->first)->products().serialize(warehouse_products);
			structure->linkEndChild(warehouse_products);
		}

		if (it->first->isRobotCommand())
		{
			XmlElement* robots = new XmlElement("robots");

			const RobotList& rl = static_cast<RobotCommand*>(it->first)->robots();

			std::stringstream str;
			for (size_t i = 0; i < rl.size(); ++i)
			{
				str << rl[i]->id();
				if (i != rl.size() - 1) { str << ","; }	// kind of a kludge
			}

			robots->attribute("robots", str.str());
			structure->linkEndChild(robots);
		}

		structures->linkEndChild(structure);
	}

	_ti->linkEndChild(structures);
}
