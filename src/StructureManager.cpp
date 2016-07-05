#include "StructureManager.h"

#include "Constants.h"
#include "StructureTranslator.h"

#include <algorithm>


StructureManager::StructureManager(): mTotalEnergyOutput(0)
{}


/**
 * D'tor
 * 
 * \note	Things (aka Structures) are deleted whenever a Tile
 *			is deleted so this d'tor really doesn't need to do
 *			anything special.
 */
StructureManager::~StructureManager()
{}


bool StructureManager::CHAPAvailable()
{
	for (size_t i = 0; i < mStructureLists[Structure::STRUCTURE_LIFE_SUPPORT].size(); ++i)
		if (mStructureLists[Structure::STRUCTURE_LIFE_SUPPORT][i]->operational())
			return true;

	return false;
}


void StructureManager::update(ResourcePool& _r)
{
	// Called separately so that 1) high priority structures can be updated first and
	// 2) so that resource handling code (like energy) can be handled between update
	// calls to lower priority structures.
	updateStructures(_r, mStructureLists[Structure::STRUCTURE_LANDER]);
	updateStructures(_r, mStructureLists[Structure::STRUCTURE_COMMAND]);
	updateStructures(_r, mStructureLists[Structure::STRUCTURE_ENERGY_PRODUCTION]);

	updateEnergyProduction(_r);

	// Basic resource production
	updateStructures(_r, mStructureLists[Structure::STRUCTURE_MINE]);
	updateStructures(_r, mStructureLists[Structure::STRUCTURE_SMELTER]);

	updateStructures(_r, mStructureLists[Structure::STRUCTURE_LIFE_SUPPORT]);
	updateStructures(_r, mStructureLists[Structure::STRUCTURE_FOOD_PRODUCTION]);

	updateStructures(_r, mStructureLists[Structure::STRUCTURE_FACTORY]);

	updateStructures(_r, mStructureLists[Structure::STRUCTURE_STORAGE]);
	updateStructures(_r, mStructureLists[Structure::STRUCTURE_RESIDENCE]);

	updateFactoryProduction();
}


void StructureManager::updateEnergyProduction(ResourcePool& _r)
{
	mTotalEnergyOutput = 0;
	for (size_t i = 0; i < mStructureLists[Structure::STRUCTURE_ENERGY_PRODUCTION].size(); ++i)
	{
		if (mStructureLists[Structure::STRUCTURE_ENERGY_PRODUCTION][i]->operational())
			mTotalEnergyOutput += mStructureLists[Structure::STRUCTURE_ENERGY_PRODUCTION][i]->resourcesOut().energy();
	}

	_r.energy(mTotalEnergyOutput);
}


void StructureManager::updateStructures(ResourcePool& _r, StructureList& _sl)
{
	bool chapAvailable = CHAPAvailable();

	Structure* structure = nullptr;
	for (size_t i = 0; i < _sl.size(); ++i)
	{
		structure = _sl[i];
		structure->update();

		// State Check
		// ASSUMPTION:	Construction sites are considered self sufficient until they are
		//				completed and connected to the rest of the colony.
		if (structure->underConstruction() || structure->destroyed())
			continue; // FIXME: smells of bad code, consider a different control path.

		// Connection Check
		if (!structureConnected(structure) && !structure->selfSustained())
		{
			structure->disable();
			continue; // FIXME: smells of bad code, consider a different control path.
		}

		// CHAP Check
		if (structure->requiresCHAP() && !chapAvailable)
		{
			structure->disable();
			continue; // FIXME: smells of bad code, consider a different control path.
		}

		// handle input resources
		if (structure->resourcesIn().empty() || structure->enoughResourcesAvailable(_r) && !structure->isIdle())
		{
			structure->enable();
			_r -= structure->resourcesIn();
		}
		else
		{
			if(!structure->isIdle())
				structure->disable();
		}

		if(structure->operational() || structure->isIdle())
			structure->think();

		// handle output resources
	}
}


void StructureManager::updateFactoryProduction()
{
	StructureList& sl = mStructureLists[Structure::STRUCTURE_FACTORY];

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
	if(t == nullptr)
		return;

	if (mStructureTileTable.find(st) != mStructureTileTable.end())
	{
		throw Exception(0, "Duplicate Structure", "StructureManager::addStructure(): Attempting to add a Structure that is already managed!");
		return;
	}

	// Remove thing's from tile only if we know we're adding a structure.
	if (!t->empty())
		t->removeThing();
	
	mStructureTileTable[st] = t;
	
	mStructureLists[st->type()].push_back(st);
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
	StructureList& sl = mStructureLists[st->type()];

	if (sl.empty())
	{
		throw Exception(0, "Rogue Structure", "StructureManager::removeStructure(): Attempting to remove a Structure that is not managed by the StructureManager.");
		return; //overkill?
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
		throw Exception(0, "Rogue Structure", "StructureManager::removeStructure(): Attempting to remove a Structure that is not managed by the StructureManager.");
		return; //overkill?
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
		st_it->second->connected(false);
}


/**
* Returns the number of structures currently being managed by the StructureManager.
*/
int StructureManager::count() const
{
	int count = 0;
	for (auto it = mStructureLists.begin(); it != mStructureLists.end(); ++it)
		count += it->second.size();
	
	return count;
}


void serializeResourcePool(TiXmlElement* _ti, ResourcePool& _rp, const std::string name)
{
	TiXmlElement* pool = new TiXmlElement(name);
	_rp.serialize(pool);
	_ti->LinkEndChild(pool);
}

void serializeStructure(TiXmlElement* _ti, Structure* _s, Tile* _t)
{
	_ti->SetAttribute("x", _t->x());
	_ti->SetAttribute("y", _t->y());
	_ti->SetAttribute("depth", _t->depth());

	_ti->SetAttribute("id", _s->id());
	_ti->SetAttribute("age", _s->age());
	_ti->SetAttribute("state", _s->state());
	//_ti->SetAttribute("type", StructureTranslator::translateFromString(_s->name()));
	_ti->SetAttribute("type", _s->name());
	_ti->SetAttribute("direction", _s->connectorDirection());

	if (!_s->production().empty())
		serializeResourcePool(_ti, _s->production(), "production");

	if (!_s->storage().empty())
		serializeResourcePool(_ti, _s->storage(), "storage");
}


void StructureManager::serialize(TiXmlElement* _ti)
{
	TiXmlElement* structures = new TiXmlElement("structures");
	TiXmlElement* factories = new TiXmlElement("factories");


	for (auto it = mStructureTileTable.begin(); it != mStructureTileTable.end(); ++it)
	{

		if (it->first->isFactory())
		{
			TiXmlElement* factory = new TiXmlElement("factory");
			serializeStructure(factory, it->first, it->second);
			Factory* f = static_cast<Factory*>(it->first);
			factory->SetAttribute("production_completed", f->productionTurnsCompleted());
			factory->SetAttribute("production_type", f->productionType());
			factories->LinkEndChild(factory);
		}
		else
		{
			TiXmlElement* structure = new TiXmlElement("structure");
			serializeStructure(structure, it->first, it->second);
			structures->LinkEndChild(structure);
		}
	}

	_ti->LinkEndChild(structures);
	_ti->LinkEndChild(factories);
}


void StructureManager::deserialize(TiXmlElement* _ti)
{


}
