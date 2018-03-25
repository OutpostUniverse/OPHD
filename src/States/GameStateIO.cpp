// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ==================================================================================
// = This file implements the File Input/Outpost used by GameState. I separated it
// = into its own file because the GameState.cpp file was starting to get a little
// = out of control.
// ==================================================================================

#include "GameState.h"


#include "../Constants.h"
#include "../StructureCatalogue.h"
#include "../StructureTranslator.h"


using namespace std;
using namespace NAS2D;
using namespace NAS2D::Xml;


/// \fixme	Fugly, find a sane way to do this.
extern const std::string MAP_TERRAIN_EXTENSION = "_a.png";
extern const std::string MAP_DISPLAY_EXTENSION = "_b.png";

extern std::string CURRENT_LEVEL_STRING;
extern std::map <int, std::string> LEVEL_STRING_TABLE;


/**
 * 
 */
void GameState::save(const std::string& _path)
{
	XmlDocument doc;

	XmlElement* root = new XmlElement(constants::SAVE_GAME_ROOT_NODE);
	root->attribute("version", constants::SAVE_GAME_VERSION);
	doc.linkEndChild(root);

	mTileMap->serialize(root);
	mStructureManager.serialize(root);
	writeRobots(root, mRobotPool, mRobotList);
	writeResources(root, mPlayerResources);

	XmlElement* turns = new XmlElement("turns");
	turns->attribute("count", mTurnCount);
	root->linkEndChild(turns);

	XmlElement* population = new XmlElement("population");
	population->attribute("morale", mCurrentMorale);
	population->attribute("prev_morale", mPreviousMorale);
	population->attribute("colonist_landers", mLandersColonist);
	population->attribute("cargo_landers", mLandersCargo);
	population->attribute("children", mPopulation.size(Population::ROLE_CHILD));
	population->attribute("students", mPopulation.size(Population::ROLE_STUDENT));
	population->attribute("workers", mPopulation.size(Population::ROLE_WORKER));
	population->attribute("scientists", mPopulation.size(Population::ROLE_SCIENTIST));
	population->attribute("retired", mPopulation.size(Population::ROLE_RETIRED));
	root->linkEndChild(population);

	XmlElement* ai = new XmlElement("ai");
	ai->attribute("gender", Utility<AiVoiceNotifier>::get().gender());
	root->linkEndChild(ai);


	// Write out the XML file.
	XmlMemoryBuffer buff;
	doc.accept(&buff);

	Utility<Filesystem>::get().write(File(buff.buffer(), _path));
}


/**
 * 
 */
void GameState::load(const std::string& _path)
{
	resetUi();

	if (!Utility<Filesystem>::get().exists(_path))
	{
		return;
	}

	File xmlFile = Utility<Filesystem>::get().open(_path);

	XmlDocument doc;

	// Load the XML document and handle any errors if occuring
	doc.parse(xmlFile.raw_bytes());
	if (doc.error())
	{
		cout << "Malformed savegame ('" << _path << "'). Error on Row " << doc.errorRow() << ", Column " << doc.errorCol() << ": " << doc.errorDesc() << endl;
		return;
	}

	XmlElement* root = doc.firstChildElement(constants::SAVE_GAME_ROOT_NODE);
	if (root == nullptr)
	{
		cout << "Root element in '" << _path << "' is not '" << constants::SAVE_GAME_ROOT_NODE << "'." << endl;
		return;
	}

	std::string sg_version = root->attribute("version");
	if (sg_version != constants::SAVE_GAME_VERSION)
	{
		cout << "Savegame version mismatch: '" << _path << "'. Expected " << constants::SAVE_GAME_VERSION << ", found " << sg_version << "." << endl;
		return;
	}

	// remove all robots currently deployed
	scrubRobotList();
	mPlayerResources.clear();
	mStructureManager.dropAllStructures();

	delete mTileMap;
	mTileMap = nullptr;

	//mTileMap->deserialize(root);
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
	mTileMap = new TileMap(sitemap, map->attribute("tset"), depth, 0, false);
	mTileMap->deserialize(root);

	readStructures(root->firstChildElement("structures"));
	readRobots(root->firstChildElement("robots"));

	readResources(root->firstChildElement("resources"), mPlayerResources);
	readPopulation(root->firstChildElement("population"));
	readTurns(root->firstChildElement("turns"));

	XmlElement* ai = root->firstChildElement("ai");
	if (ai)
	{
		int gender = 0;
		ai->firstAttribute()->queryIntValue(gender);
		Utility<AiVoiceNotifier>::get().gender(static_cast<AiVoiceNotifier::AiGender>(gender));
	}

	mPlayerResources.capacity(totalStorage(mStructureManager.structureList(Structure::CLASS_STORAGE)));

	checkConnectedness();
	mStructureManager.updateEnergyProduction(mPlayerResources, mPopulationPool);

	updateRobotControl(mRobotPool, mStructureManager);
	updateResidentialCapacity();

	// set level indicator string
	CURRENT_LEVEL_STRING = LEVEL_STRING_TABLE[mTileMap->currentDepth()];
}


/**
 * 
 */
void GameState::readRobots(XmlElement* _ti)
{
	mRobotPool.clear();
	mRobotList.clear();
	mRobots.dropAllItems();

	int type = 0, age = 0, production_time = 0, x = 0, y = 0, depth = 0, direction = 0;
	XmlAttribute* attribute = nullptr;
	for (XmlNode* robot = _ti->firstChild(); robot; robot = robot->nextSibling())
	{
		type = 0, age = 0, production_time = 0, x = 0, y = 0, depth = 0, direction = 0;
		attribute = robot->toElement()->firstAttribute();
		while (attribute)
		{
			if (attribute->name() == "type") { attribute->queryIntValue(type); }
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
		case ROBOT_DIGGER:
			r = mRobotPool.addRobot(ROBOT_DIGGER);
			r->taskComplete().connect(this, &GameState::diggerTaskFinished);
			static_cast<Robodigger*>(r)->direction(static_cast<Direction>(direction));
			break;

		case ROBOT_DOZER:
			r = mRobotPool.addRobot(ROBOT_DOZER);
			r->taskComplete().connect(this, &GameState::dozerTaskFinished);
			break;

		case ROBOT_MINER:
			r = mRobotPool.addRobot(ROBOT_MINER);
			r->taskComplete().connect(this, &GameState::minerTaskFinished);
			break;

		default:
			cout << "Unknown robot type in savegame." << endl;
			break;
		}

		if (!r) { continue; }	// Could be done in the default handler in the above switch
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

	if (mRobotPool.robotAvailable(ROBOT_DIGGER)) { checkRobotSelectionInterface(constants::ROBODIGGER, constants::ROBODIGGER_SHEET_ID, ROBOT_DIGGER); }
	if (mRobotPool.robotAvailable(ROBOT_DOZER)) { checkRobotSelectionInterface(constants::ROBODOZER, constants::ROBODOZER_SHEET_ID, ROBOT_DOZER); }
	if (mRobotPool.robotAvailable(ROBOT_MINER)) { checkRobotSelectionInterface(constants::ROBOMINER, constants::ROBOMINER_SHEET_ID, ROBOT_MINER); }
}


void GameState::readStructures(XmlElement* _ti)
{
	std::string type;
	int x = 0, y = 0, depth = 0, id = 0, age = 0, state = 0, direction = 0, forced_idle = 0;
	int production_completed = 0, production_type = 0;
	XmlAttribute* attribute = nullptr;
	for (XmlNode* structure = _ti->firstChild(); structure != nullptr; structure = structure->nextSibling())
	{
		x = y = depth = id = age = state = direction = production_completed = production_type = 0;
		attribute = structure->toElement()->firstAttribute();
		while (attribute)
		{
			if (attribute->name() == "x") { attribute->queryIntValue(x); }
			else if (attribute->name() == "y") { attribute->queryIntValue(y); }
			else if (attribute->name() == "depth") { attribute->queryIntValue(depth); }
			else if (attribute->name() == "id") { attribute->queryIntValue(id); }
			else if (attribute->name() == "age") { attribute->queryIntValue(age); }
			else if (attribute->name() == "state") { attribute->queryIntValue(state); }
			else if (attribute->name() == "direction") { attribute->queryIntValue(direction); }
			else if (attribute->name() == "type") { type = attribute->value(); }
			else if (attribute->name() == "forced_idle") { attribute->queryIntValue(forced_idle); }

			else if (attribute->name() == "production_completed") { attribute->queryIntValue(production_completed); }
			else if (attribute->name() == "production_type") { attribute->queryIntValue(production_type); }

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

		if (type_id == SID_COMMAND_CENTER)
		{
			mCCLocation(x, y);
		}

		if (type_id == SID_MINE_FACILITY)
		{
			Mine* m = mTileMap->getTile(x, y, 0)->mine();
			if (m == nullptr)
			{
				throw runtime_error("Mine Facility is located on a Tile with no Mine.");
			}

			static_cast<MineFacility*>(st)->mine(m);
		}

		if (type_id == SID_AIR_SHAFT && depth > 0)
		{
			static_cast<AirShaft*>(st)->ug(); // force underground state
		}

		st->age(age);
		st->id(id);
		st->forced_state_change(static_cast<Structure::StructureState>(state));
		st->connectorDirection(static_cast<ConnectorDir>(direction));
		st->forceIdle(forced_idle != 0);

		st->production().deserialize(structure->firstChildElement("production"));
		st->storage().deserialize(structure->firstChildElement("storage"));

		if (st->isFactory())
		{
			Factory* f = static_cast<Factory*>(st);
			f->productType(static_cast<ProductType>(production_type));
			f->productionTurnsCompleted(production_completed);
			f->resourcePool(&mPlayerResources);
			f->productionComplete().connect(this, &GameState::factoryProductionComplete);
		}

		mStructureManager.addStructure(st, t);
	}
}


/**
 * 
 */
void GameState::readTurns(XmlElement* _ti)
{
	if (_ti)
	{
		_ti->firstAttribute()->queryIntValue(mTurnCount);

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
void GameState::readPopulation(XmlElement* _ti)
{
	if (_ti)
	{
		mPopulation.clear();

		int children = 0, students = 0, workers = 0, scientists = 0, retired = 0;

		XmlAttribute* attribute = _ti->firstAttribute();
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

		mPopulation.addPopulation(Population::ROLE_CHILD, children);
		mPopulation.addPopulation(Population::ROLE_STUDENT, students);
		mPopulation.addPopulation(Population::ROLE_WORKER, workers);
		mPopulation.addPopulation(Population::ROLE_SCIENTIST, scientists);
		mPopulation.addPopulation(Population::ROLE_RETIRED, retired);
	}
}
