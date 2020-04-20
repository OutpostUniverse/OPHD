// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ==================================================================================
// = This file implements the File Input/Outpost used by MapViewState. I separated it
// = into its own file because the MapViewState.cpp file was starting to get a little
// = out of control.
// ==================================================================================

#include "MapViewState.h"


#include "../Constants.h"
#include "../StructureCatalogue.h"
#include "../StructureTranslator.h"

#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "NAS2D/Xml/XmlDocument.h"
#include "NAS2D/Xml/XmlMemoryBuffer.h"

using namespace NAS2D;
using namespace NAS2D::Xml;


/// \fixme	Fugly, find a sane way to do this.
extern const std::string MAP_TERRAIN_EXTENSION = "_a.png";
extern const std::string MAP_DISPLAY_EXTENSION = "_b.png";

extern std::string CURRENT_LEVEL_STRING;
extern std::map <int, std::string> LEVEL_STRING_TABLE;


extern NAS2D::Image* IMG_LOADING; /// \fixme	Hate having these as externs.
extern NAS2D::Image* IMG_SAVING;


extern int ROBOT_ID_COUNTER; /// \fixme Kludge


/**
 * 
 */
void MapViewState::save(const std::string& filePath)
{
	auto& renderer = Utility<Renderer>::get();
	renderer.drawBoxFilled(0, 0, renderer.width(), renderer.height(), 0, 0, 0, 100);
	renderer.drawImage(*IMG_SAVING, renderer.center() - IMG_SAVING->size() / 2);
	renderer.update();

	XmlDocument doc;

	XmlElement* root = new XmlElement(constants::SAVE_GAME_ROOT_NODE);
	root->attribute("version", constants::SAVE_GAME_VERSION);
	doc.linkEndChild(root);

	mTileMap->serialize(root);
	Utility<StructureManager>::get().serialize(root);
	writeRobots(root, mRobotPool, mRobotList);
	writeResources(root, mPlayerResources, "resources");
	writeResources(root, mResourceBreakdownPanel.previousResources(), "prev_resources");

	XmlElement* turns = new XmlElement("turns");
	turns->attribute("count", mTurnCount);
	root->linkEndChild(turns);

	XmlElement* population = new XmlElement("population");
	population->attribute("morale", mCurrentMorale);
	population->attribute("prev_morale", mPreviousMorale);
	population->attribute("colonist_landers", mLandersColonist);
	population->attribute("cargo_landers", mLandersCargo);
	population->attribute("children", mPopulation.size(Population::PersonRole::ROLE_CHILD));
	population->attribute("students", mPopulation.size(Population::PersonRole::ROLE_STUDENT));
	population->attribute("workers", mPopulation.size(Population::PersonRole::ROLE_WORKER));
	population->attribute("scientists", mPopulation.size(Population::PersonRole::ROLE_SCIENTIST));
	population->attribute("retired", mPopulation.size(Population::PersonRole::ROLE_RETIRED));
	root->linkEndChild(population);

	// Write out the XML file.
	XmlMemoryBuffer buff;
	doc.accept(&buff);

	Utility<Filesystem>::get().write(File(buff.buffer(), filePath));
}


using namespace micropather;
extern MicroPather* pather;
extern std::vector<void*> path;

/**
 * 
 */
void MapViewState::load(const std::string& filePath)
{
	resetUi();

	auto& renderer = Utility<Renderer>::get();
	renderer.drawBoxFilled(0, 0, renderer.width(), renderer.height(), 0, 0, 0, 100);
	renderer.drawImage(*IMG_LOADING, renderer.center() - IMG_LOADING->size() / 2);
	renderer.update();

	mBtnToggleConnectedness.toggle(false);
	mBtnToggleHeightmap.toggle(false);


	if (!Utility<Filesystem>::get().exists(filePath))
	{
		throw std::runtime_error("File '" + filePath + "' was not found.");
	}

	File xmlFile = Utility<Filesystem>::get().open(filePath);

	XmlDocument doc;

	// Load the XML document and handle any errors if occuring
	doc.parse(xmlFile.raw_bytes());
	if (doc.error())
	{
		throw std::runtime_error("Malformed savegame ('" + filePath + "'). Error on Row " + std::to_string(doc.errorRow()) + ", Column " + std::to_string(doc.errorCol()) + ": " + doc.errorDesc());
	}

	XmlElement* root = doc.firstChildElement(constants::SAVE_GAME_ROOT_NODE);
	if (root == nullptr)
	{
		throw std::runtime_error("Root element in '" + filePath + "' is not '" + constants::SAVE_GAME_ROOT_NODE + "'.");
	}

	std::string sg_version = root->attribute("version");
	if (sg_version != constants::SAVE_GAME_VERSION)
	{
		throw std::runtime_error("Savegame version mismatch: '" + filePath + "'. Expected " + constants::SAVE_GAME_VERSION + ", found " + sg_version + ".");
	}

	scrubRobotList();
	mPlayerResources.clear();
	Utility<StructureManager>::get().dropAllStructures();
	ccLocation() = CcNotPlaced;

	delete mTileMap;
	mTileMap = nullptr;

	XmlElement* map = root->firstChildElement("properties");
	int depth = 0;
	std::string sitemap;
	XmlAttribute* attribute = map->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == "diggingdepth") { attribute->queryIntValue(depth); }
		else if (attribute->name() == "sitemap") { sitemap = attribute->value(); }
		attribute = attribute->next();
	}

	mMapDisplay = Image(sitemap + MAP_DISPLAY_EXTENSION);
	mHeightMap = Image(sitemap + MAP_TERRAIN_EXTENSION);
	mTileMap = new TileMap(sitemap, map->attribute("tset"), depth, 0, constants::PlanetHostility::HOSTILITY_NONE, false);
	mTileMap->deserialize(root);

	delete pather;
	pather = new MicroPather(mTileMap);
	path.clear();


	/**
	 * In the case of loading a game, the Robot Command Center depends on the robot list
	 * having already been loaded in order to match up the robots in the save game to
	 * the RCC.
	 */
	readRobots(root->firstChildElement("robots"));
	readStructures(root->firstChildElement("structures"));

	readResources(root->firstChildElement("resources"), mPlayerResources);
	readResources(root->firstChildElement("prev_resources"), mResourceBreakdownPanel.previousResources());
	readPopulation(root->firstChildElement("population"));
	readTurns(root->firstChildElement("turns"));

	mPlayerResources.capacity(totalStorage(Utility<StructureManager>::get().structureList(Structure::StructureClass::CLASS_STORAGE)));

	checkConnectedness();

	/**
	 * StructureManager::updateEnergyProduction() overwrites the energy count in the player resource
	 * pool so we store the original value here and set it after counting the total energy available.
	 * Kind of a kludge.
	 */
	int energy = mPlayerResources.energy();
	Utility<StructureManager>::get().updateEnergyProduction(mPlayerResources, mPopulationPool);
	mPlayerResources.energy(energy);

	updateRobotControl(mRobotPool);
	updateResidentialCapacity();

	if (mTurnCount == 0)
	{
		if (Utility<StructureManager>::get().count() == 0)
		{
			mBtnTurns.enabled(false);
			populateStructureMenu();
		}
		else
		{
			/**
			 * There should only ever be one structure if the turn count is 0, the
			 * SEED Lander which at this point should not have been deployed.
			 */
			StructureList& list = Utility<StructureManager>::get().structureList(Structure::StructureClass::CLASS_LANDER);
			if (list.size() != 1) { throw std::runtime_error("MapViewState::load(): Turn counter at 0 but more than one structure in list."); }

			SeedLander* s = dynamic_cast<SeedLander*>(list[0]);
			if (!s) { throw std::runtime_error("MapViewState::load(): Structure in list is not a SeedLander."); }

			s->deployCallback().connect(this, &MapViewState::deploySeedLander);

			mStructures.dropAllItems();
			mConnections.dropAllItems();
			mBtnTurns.enabled(true);
		}
	}
	else
	{
		populateStructureMenu();
	}

	CURRENT_LEVEL_STRING = LEVEL_STRING_TABLE[mTileMap->currentDepth()];
	mResourceBreakdownPanel.resourceCheck();

	mMapChangedCallback();
}


/**
 * 
 */
void MapViewState::readRobots(Xml::XmlElement* element)
{
	mRobotPool.clear();
	mRobotList.clear();
	mRobots.dropAllItems();

	/**
	 * \fixme	This is fragile and prone to break if the savegame file is malformed.
	 */
	element->firstAttribute()->queryIntValue(ROBOT_ID_COUNTER);

	int id = 0, type = 0, age = 0, production_time = 0, x = 0, y = 0, depth = 0, direction = 0;
	XmlAttribute* attribute = nullptr;
	for (XmlNode* robot = element->firstChild(); robot; robot = robot->nextSibling())
	{
		id = type = age = production_time = x = y = depth = direction = 0;
		attribute = robot->toElement()->firstAttribute();
		while (attribute)
		{
			if (attribute->name() == "id") { attribute->queryIntValue(id); }
			else if (attribute->name() == "type") { attribute->queryIntValue(type); }
			else if (attribute->name() == "age") { attribute->queryIntValue(age); }
			else if (attribute->name() == "production") { attribute->queryIntValue(production_time); }
			else if (attribute->name() == "x") { attribute->queryIntValue(x); }
			else if (attribute->name() == "y") { attribute->queryIntValue(y); }
			else if (attribute->name() == "depth") { attribute->queryIntValue(depth); }
			else if (attribute->name() == "direction") { attribute->queryIntValue(direction); }

			attribute = attribute->next();
		}

		Robot* r = nullptr;
		switch (static_cast<RobotType>(type))
		{
		case RobotType::ROBOT_DIGGER:
			r = mRobotPool.addRobot(RobotType::ROBOT_DIGGER, id);
			r->taskComplete().connect(this, &MapViewState::diggerTaskFinished);
			static_cast<Robodigger*>(r)->direction(static_cast<Direction>(direction));
			break;

		case RobotType::ROBOT_DOZER:
			r = mRobotPool.addRobot(RobotType::ROBOT_DOZER, id);
			r->taskComplete().connect(this, &MapViewState::dozerTaskFinished);
			break;

		case RobotType::ROBOT_MINER:
			r = mRobotPool.addRobot(RobotType::ROBOT_MINER, id);
			r->taskComplete().connect(this, &MapViewState::minerTaskFinished);
			break;

		default:
			std::cout << "Unknown robot type in savegame." << std::endl;
			break;
		}

		if (!r) { continue; } // Could be done in the default handler in the above switch
								// but may be better here as an explicit statement.

		r->fuelCellAge(age);

		if (production_time > 0)
		{
			r->startTask(production_time);
			mRobotPool.insertRobotIntoTable(mRobotList, r, mTileMap->getTile(x, y, depth));
			mRobotList[r]->index(0);
		}

		if (depth > 0)
		{
			mRobotList[r]->excavated(true);
		}
	}

	if (mRobotPool.robotAvailable(RobotType::ROBOT_DIGGER)) { checkRobotSelectionInterface(constants::ROBODIGGER, constants::ROBODIGGER_SHEET_ID, RobotType::ROBOT_DIGGER); }
	if (mRobotPool.robotAvailable(RobotType::ROBOT_DOZER)) { checkRobotSelectionInterface(constants::ROBODOZER, constants::ROBODOZER_SHEET_ID, RobotType::ROBOT_DOZER); }
	if (mRobotPool.robotAvailable(RobotType::ROBOT_MINER)) { checkRobotSelectionInterface(constants::ROBOMINER, constants::ROBOMINER_SHEET_ID, RobotType::ROBOT_MINER); }
}


void MapViewState::readStructures(Xml::XmlElement* element)
{
	std::string type;
	int x = 0, y = 0, depth = 0, age = 0, state = 0, direction = 0, forced_idle = 0, disabled_reason = 0, idle_reason = 0, pop0 = 0, pop1 = 0;
	int production_completed = 0, production_type = 0;
	XmlAttribute* attribute = nullptr;
	for (XmlNode* structure = element->firstChild(); structure != nullptr; structure = structure->nextSibling())
	{
		x = y = depth = age = state = direction = production_completed = production_type = disabled_reason = idle_reason = pop0 = pop1 = 0;
		attribute = structure->toElement()->firstAttribute();
		while (attribute)
		{
			if (attribute->name() == "x") { attribute->queryIntValue(x); }
			else if (attribute->name() == "y") { attribute->queryIntValue(y); }
			else if (attribute->name() == "depth") { attribute->queryIntValue(depth); }
			else if (attribute->name() == "age") { attribute->queryIntValue(age); }
			else if (attribute->name() == "state") { attribute->queryIntValue(state); }
			else if (attribute->name() == "direction") { attribute->queryIntValue(direction); }
			else if (attribute->name() == "type") { type = attribute->value(); }
			else if (attribute->name() == "forced_idle") { attribute->queryIntValue(forced_idle); }
			else if (attribute->name() == "disabled_reason") { attribute->queryIntValue(disabled_reason); }
			else if (attribute->name() == "idle_reason") { attribute->queryIntValue(idle_reason); }

			else if (attribute->name() == "production_completed") { attribute->queryIntValue(production_completed); }
			else if (attribute->name() == "production_type") { attribute->queryIntValue(production_type); }

			else if (attribute->name() == "pop0") { attribute->queryIntValue(pop0); }
			else if (attribute->name() == "pop1") { attribute->queryIntValue(pop1); }

			attribute = attribute->next();
		}

		Tile* t = mTileMap->getTile(x, y, depth);
		t->index(0);
		t->excavated(true);

		Structure* st = nullptr;
		// case for tubes
		if (type == constants::TUBE)
		{
			ConnectorDir cd = static_cast<ConnectorDir>(direction);
			insertTube(cd, depth, mTileMap->getTile(x, y, depth));
			continue; // FIXME: ugly
		}

		StructureID type_id = StructureTranslator::translateFromString(type);
		st = StructureCatalogue::get(type_id);

		if (type_id == StructureID::SID_COMMAND_CENTER)
		{
			ccLocation() = {x, y};
		}

		if (type_id == StructureID::SID_MINE_FACILITY)
		{
			Mine* m = mTileMap->getTile(x, y, 0)->mine();
			if (m == nullptr)
			{
				throw std::runtime_error("Mine Facility is located on a Tile with no Mine.");
			}

			MineFacility* mf = static_cast<MineFacility*>(st);
			mf->mine(m);
			mf->maxDepth(mTileMap->maxDepth());
			mf->extensionComplete().connect(this, &MapViewState::mineFacilityExtended);
		}

		if (type_id == StructureID::SID_AIR_SHAFT && depth != 0)
		{
			static_cast<AirShaft*>(st)->ug(); // force underground state
		}

		if (type_id == StructureID::SID_SEED_LANDER)
		{
			static_cast<SeedLander*>(st)->position(x, y);
		}

		st->age(age);
		st->forced_state_change(static_cast<Structure::StructureState>(state), static_cast<DisabledReason>(disabled_reason), static_cast<IdleReason>(idle_reason));
		st->connectorDirection(static_cast<ConnectorDir>(direction));
		
		if (forced_idle != 0) { st->forceIdle(forced_idle != 0); }

		st->production().deserialize(structure->firstChildElement("production"));
		st->storage().deserialize(structure->firstChildElement("storage"));

		if (st->isWarehouse())
		{
			Warehouse* w = static_cast<Warehouse*>(st);
			w->products().deserialize(structure->firstChildElement("warehouse_products"));
		}

		if (st->isFactory())
		{
			Factory* f = static_cast<Factory*>(st);
			f->productType(static_cast<ProductType>(production_type));
			f->productionTurnsCompleted(production_completed);
			f->resourcePool(&mPlayerResources);
			f->productionComplete().connect(this, &MapViewState::factoryProductionComplete);
		}

		/**
		 * This is a little fragile in that it assumes that everything it expects is
		 * encoded in the XML savegame. While there are some basic guards in place when
		 * loading the code doesn't do any checking for garbage for the sake of brevity.
		 */
		if (st->isRobotCommand())
		{
			RobotCommand* rcc = static_cast<RobotCommand*>(st);
			XmlAttribute* robots = structure->firstChildElement("robots")->firstAttribute();

			if (!robots->value().empty())
			{
				StringList rl_str = split_string(robots->value().c_str(), ',');

				const RobotList& rl = mRobotPool.robots();

				for (std::size_t i = 0; i < rl_str.size(); ++i)
				{
					int _rid = std::stoi(rl_str[i]);
					for (std::size_t ri = 0; ri < rl.size(); ++ri)
					{
						if (rl[ri]->id() == _rid)
						{
							rcc->addRobot(rl[ri]);
							break;
						}
					}
				}
			}
		}

		st->populationAvailable()[0] = pop0;
		st->populationAvailable()[1] = pop1;

		Utility<StructureManager>::get().addStructure(st, t);
	}
}


/**
 * 
 */
void MapViewState::readTurns(Xml::XmlElement* element)
{
	if (element)
	{
		element->firstAttribute()->queryIntValue(mTurnCount);

		if (mTurnCount > 0)
		{
			mBtnTurns.enabled(true);
			populateStructureMenu();
		}
	}
}


/**
 * Reads the population tag.
 */
void MapViewState::readPopulation(Xml::XmlElement* element)
{
	if (element)
	{
		mPopulation.clear();

		int children = 0, students = 0, workers = 0, scientists = 0, retired = 0;

		XmlAttribute* attribute = element->firstAttribute();
		while (attribute)
		{
			if (attribute->name() == "morale") { attribute->queryIntValue(mCurrentMorale); }
			else if (attribute->name() == "prev_morale") { attribute->queryIntValue(mPreviousMorale); }
			else if (attribute->name() == "colonist_landers") { attribute->queryIntValue(mLandersColonist); }
			else if (attribute->name() == "cargo_landers") { attribute->queryIntValue(mLandersCargo); }

			else if (attribute->name() == "children") { attribute->queryIntValue(children); }
			else if (attribute->name() == "students") { attribute->queryIntValue(students); }
			else if (attribute->name() == "workers") { attribute->queryIntValue(workers); }
			else if (attribute->name() == "scientists") { attribute->queryIntValue(scientists); }
			else if (attribute->name() == "retired") { attribute->queryIntValue(retired); }

			attribute = attribute->next();
		}

		mPopulation.addPopulation(Population::PersonRole::ROLE_CHILD, children);
		mPopulation.addPopulation(Population::PersonRole::ROLE_STUDENT, students);
		mPopulation.addPopulation(Population::PersonRole::ROLE_WORKER, workers);
		mPopulation.addPopulation(Population::PersonRole::ROLE_SCIENTIST, scientists);
		mPopulation.addPopulation(Population::PersonRole::ROLE_RETIRED, retired);
	}
}
