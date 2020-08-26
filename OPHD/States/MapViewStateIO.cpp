// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ==================================================================================
// = This file implements the File Input/Outpost used by MapViewState. I separated it
// = into its own file because the MapViewState.cpp file was starting to get a little
// = out of control.
// ==================================================================================

#include "MapViewState.h"

#include "../Cache.h"
#include "../Constants.h"
#include "../IOHelper.h"
#include "../StructureCatalogue.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/StringUtils.h>
#include <NAS2D/Xml/XmlDocument.h>
#include <NAS2D/Xml/XmlMemoryBuffer.h>

#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

using namespace NAS2D;
using namespace NAS2D::Xml;


/// \fixme	Fugly, find a sane way to do this.
extern const std::string MAP_TERRAIN_EXTENSION = "_a.png";
extern const std::string MAP_DISPLAY_EXTENSION = "_b.png";

extern std::string CURRENT_LEVEL_STRING;
extern std::map <int, std::string> LEVEL_STRING_TABLE;


extern int ROBOT_ID_COUNTER; /// \fixme Kludge



/*****************************************************************************
 * LOCAL FUNCTIONS
 *****************************************************************************/
static void loadResorucesFromXmlElement(NAS2D::Xml::XmlElement* element, StorableResources& resources)
{
	if (!element) { return; }

	resources.resources[0] = std::stoi(element->attribute(constants::SAVE_GAME_RESOURCE_0));
	resources.resources[1] = std::stoi(element->attribute(constants::SAVE_GAME_RESOURCE_1));
	resources.resources[2] = std::stoi(element->attribute(constants::SAVE_GAME_RESOURCE_2));
	resources.resources[3] = std::stoi(element->attribute(constants::SAVE_GAME_RESOURCE_3));
}


/*****************************************************************************
 * CLASS FUNCTIONS
 *****************************************************************************/

/**
 * 
 */
void MapViewState::save(const std::string& filePath)
{
	auto& renderer = Utility<Renderer>::get();
	renderer.drawBoxFilled(NAS2D::Rectangle{0, 0, renderer.size().x, renderer.size().y}, NAS2D::Color{0, 0, 0, 100});
	const auto imageSaving = &imageCache.load("sys/saving.png");
	renderer.drawImage(*imageSaving, renderer.center() - imageSaving->size() / 2);
	renderer.update();

	XmlDocument doc;

	XmlElement* root = new XmlElement(constants::SAVE_GAME_ROOT_NODE);
	root->attribute("version", constants::SAVE_GAME_VERSION);
	doc.linkEndChild(root);

	mTileMap->serialize(root, mPlanetAttributes);
	Utility<StructureManager>::get().serialize(root);
	writeRobots(root, mRobotPool, mRobotList);
	writeResources(root, mPlayerResources, "resources");
	writeResources(root, mResourceBreakdownPanel.previousResources(), "prev_resources");

	XmlElement* turns = new XmlElement("turns");
	turns->attribute("count", mTurnCount);
	root->linkEndChild(turns);

	auto energy = new XmlElement("energy");
	energy->attribute("energy", mEnergy);
	root->linkEndChild(energy);

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


/**
 * 
 */
void MapViewState::load(const std::string& filePath)
{
	mPlanetAttributes = Planet::Attributes();
	resetUi();

	auto& renderer = Utility<Renderer>::get();
	renderer.drawBoxFilled(NAS2D::Rectangle{0, 0, renderer.size().x, renderer.size().y}, NAS2D::Color{0, 0, 0, 100});
	const auto imageLoading = &imageCache.load("sys/loading.png");
	renderer.drawImage(*imageLoading, renderer.center() - imageLoading->size() / 2);
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
	XmlAttribute* attribute = map->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == "diggingdepth") { attribute->queryIntValue(mPlanetAttributes.maxDepth); }
		else if (attribute->name() == "sitemap") { mPlanetAttributes.mapImagePath = attribute->value(); }
		else if (attribute->name() == "tset") { mPlanetAttributes.tilesetPath = attribute->value(); }
		else if (attribute->name() == "meansolardistance") { mPlanetAttributes.meanSolarDistance = std::stof(attribute->value()); }
		attribute = attribute->next();
	}

	StructureCatalogue::init(mPlanetAttributes.meanSolarDistance);
	mMapDisplay = std::make_unique<Image>(mPlanetAttributes.mapImagePath + MAP_DISPLAY_EXTENSION);
	mHeightMap = std::make_unique<Image>(mPlanetAttributes.mapImagePath + MAP_TERRAIN_EXTENSION);
	mTileMap = new TileMap(mPlanetAttributes.mapImagePath, mPlanetAttributes.tilesetPath, mPlanetAttributes.maxDepth, 0, Planet::Hostility::None, false);
	mTileMap->deserialize(root);

	delete mPathSolver;
	mPathSolver = new micropather::MicroPather(mTileMap);

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

	auto energy = root->firstChildElement("energy");
	if (!energy) { throw std::runtime_error("MapViewState::load(): Savegame file is missing '<energy>' tag."); }
	mEnergy = std::stoi(energy->attribute(constants::SAVE_GAME_ENERGY));

	mRefinedResourcesCap = totalStorage(Structure::StructureClass::Storage, StorageTanksCapacity);

	checkConnectedness();

	Utility<StructureManager>::get().updateEnergyProduction();

	updateRobotControl(mRobotPool);
	updateResidentialCapacity();
	updateStructuresAvailability();
	updateFood();

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
			StructureList& list = Utility<StructureManager>::get().structureList(Structure::StructureClass::Lander);
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
	for (XmlNode* robotNode = element->firstChild(); robotNode; robotNode = robotNode->nextSibling())
	{
		id = type = age = production_time = x = y = depth = direction = 0;
		attribute = robotNode->toElement()->firstAttribute();
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

		Robot* robot = nullptr;
		switch (static_cast<RobotType>(type))
		{
		case RobotType::ROBOT_DIGGER:
			robot = mRobotPool.addRobot(RobotType::ROBOT_DIGGER, id);
			robot->taskComplete().connect(this, &MapViewState::diggerTaskFinished);
			static_cast<Robodigger*>(robot)->direction(static_cast<Direction>(direction));
			break;

		case RobotType::ROBOT_DOZER:
			robot = mRobotPool.addRobot(RobotType::ROBOT_DOZER, id);
			robot->taskComplete().connect(this, &MapViewState::dozerTaskFinished);
			break;

		case RobotType::ROBOT_MINER:
			robot = mRobotPool.addRobot(RobotType::ROBOT_MINER, id);
			robot->taskComplete().connect(this, &MapViewState::minerTaskFinished);
			break;

		default:
			std::cout << "Unknown robot type in savegame." << std::endl;
			break;
		}

		if (!robot) { continue; } // Could be done in the default handler in the above switch
								// but may be better here as an explicit statement.

		robot->fuelCellAge(age);

		if (production_time > 0)
		{
			robot->startTask(production_time);
			mRobotPool.insertRobotIntoTable(mRobotList, robot, &mTileMap->getTile({x, y}, depth));
			mRobotList[robot]->index(TerrainType::Dozed);
		}

		if (depth > 0)
		{
			mRobotList[robot]->excavated(true);
		}
	}

	if (mRobotPool.robotAvailable(RobotType::ROBOT_DIGGER)) { checkRobotSelectionInterface(constants::ROBODIGGER, constants::ROBODIGGER_SHEET_ID, RobotType::ROBOT_DIGGER); }
	if (mRobotPool.robotAvailable(RobotType::ROBOT_DOZER)) { checkRobotSelectionInterface(constants::ROBODOZER, constants::ROBODOZER_SHEET_ID, RobotType::ROBOT_DOZER); }
	if (mRobotPool.robotAvailable(RobotType::ROBOT_MINER)) { checkRobotSelectionInterface(constants::ROBOMINER, constants::ROBOMINER_SHEET_ID, RobotType::ROBOT_MINER); }
}


void MapViewState::readStructures(Xml::XmlElement* element)
{
	int x = 0, y = 0, depth = 0, age = 0, state = 0, direction = 0, forced_idle = 0, disabled_reason = 0, idle_reason = 0, pop0 = 0, pop1 = 0, type = 0;
	int production_completed = 0, production_type = 0;
	XmlAttribute* attribute = nullptr;
	for (XmlNode* structureNode = element->firstChild(); structureNode != nullptr; structureNode = structureNode->nextSibling())
	{
		x = y = depth = age = state = direction = production_completed = production_type = disabled_reason = idle_reason = pop0 = pop1 = type = 0;
		attribute = structureNode->toElement()->firstAttribute();
		while (attribute)
		{
			if (attribute->name() == "x") { attribute->queryIntValue(x); }
			else if (attribute->name() == "y") { attribute->queryIntValue(y); }
			else if (attribute->name() == "depth") { attribute->queryIntValue(depth); }
			else if (attribute->name() == "age") { attribute->queryIntValue(age); }
			else if (attribute->name() == "state") { attribute->queryIntValue(state); }
			else if (attribute->name() == "direction") { attribute->queryIntValue(direction); }
			else if (attribute->name() == "type") { attribute->queryIntValue(type); }
			else if (attribute->name() == "forced_idle") { attribute->queryIntValue(forced_idle); }
			else if (attribute->name() == "disabled_reason") { attribute->queryIntValue(disabled_reason); }
			else if (attribute->name() == "idle_reason") { attribute->queryIntValue(idle_reason); }

			else if (attribute->name() == "production_completed") { attribute->queryIntValue(production_completed); }
			else if (attribute->name() == "production_type") { attribute->queryIntValue(production_type); }

			else if (attribute->name() == "pop0") { attribute->queryIntValue(pop0); }
			else if (attribute->name() == "pop1") { attribute->queryIntValue(pop1); }

			attribute = attribute->next();
		}

		auto& tile = mTileMap->getTile({x, y}, depth);
		tile.index(TerrainType::Dozed);
		tile.excavated(true);

		auto structureId = static_cast<StructureID>(type);
		if (structureId == StructureID::SID_TUBE)
		{
			ConnectorDir connectorDir = static_cast<ConnectorDir>(direction);
			insertTube(connectorDir, depth, &mTileMap->getTile({x, y}, depth));
			continue; // FIXME: ugly
		}

		auto& structure = *StructureCatalogue::get(structureId);

		if (structureId == StructureID::SID_COMMAND_CENTER)
		{
			ccLocation() = {x, y};
		}

		if (structureId == StructureID::SID_MINE_FACILITY)
		{
			auto* mine = mTileMap->getTile({x, y}, 0).mine();
			if (mine == nullptr)
			{
				throw std::runtime_error("Mine Facility is located on a Tile with no Mine.");
			}

			auto& mineFacility = *static_cast<MineFacility*>(&structure);
			mineFacility.mine(mine);
			mineFacility.maxDepth(mTileMap->maxDepth());
			mineFacility.extensionComplete().connect(this, &MapViewState::mineFacilityExtended);
		}

		if (structureId == StructureID::SID_AIR_SHAFT && depth != 0)
		{
			static_cast<AirShaft*>(&structure)->ug(); // force underground state
		}

		if (structureId == StructureID::SID_SEED_LANDER)
		{
			static_cast<SeedLander*>(&structure)->position({x, y});
		}

		if (structureId == StructureID::SID_AGRIDOME)
		{
			auto& agridome = *static_cast<Agridome*>(&structure);

			auto foodStorage = structureNode->firstChildElement("food");
			if (foodStorage == nullptr)
			{
				throw std::runtime_error("MapViewState::readStructures(): Agridome saved without a food level node.");
			}

			auto foodLevel = foodStorage->attribute("level");
			agridome.foodLevel(std::stoi(foodLevel));
		}

		structure.age(age);
		structure.forced_state_change(static_cast<StructureState>(state), static_cast<DisabledReason>(disabled_reason), static_cast<IdleReason>(idle_reason));
		structure.connectorDirection(static_cast<ConnectorDir>(direction));

		if (forced_idle != 0) { structure.forceIdle(forced_idle != 0); }

		loadResorucesFromXmlElement(structureNode->firstChildElement("production"), structure.production());
		loadResorucesFromXmlElement(structureNode->firstChildElement("storage"), structure.storage());

		if (structure.isWarehouse())
		{
			auto& warehouse = *static_cast<Warehouse*>(&structure);
			warehouse.products().deserialize(structureNode->firstChildElement("warehouse_products"));
		}

		if (structure.isFactory())
		{
			auto& factory = *static_cast<Factory*>(&structure);
			factory.productType(static_cast<ProductType>(production_type));
			factory.productionTurnsCompleted(production_completed);
			factory.resourcePool(&mPlayerResources);
			factory.productionComplete().connect(this, &MapViewState::factoryProductionComplete);
		}

		/**
		 * This is a little fragile in that it assumes that everything it expects is
		 * encoded in the XML savegame. While there are some basic guards in place when
		 * loading the code doesn't do any checking for garbage for the sake of brevity.
		 */
		if (structure.isRobotCommand())
		{
			auto& rcc = *static_cast<RobotCommand*>(&structure);
			XmlAttribute* robotsAttribute = structureNode->firstChildElement("robots")->firstAttribute();

			for (const auto& string : NAS2D::split(robotsAttribute->value(), ','))
			{
				const auto robotId = NAS2D::stringTo<int>(string);
				for (auto* robot : mRobotPool.robots())
				{
					if (robot->id() == robotId)
					{
						rcc.addRobot(robot);
						break;
					}
				}
			}
		}

		structure.populationAvailable()[0] = pop0;
		structure.populationAvailable()[1] = pop1;

		Utility<StructureManager>::get().addStructure(&structure, &tile);
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
