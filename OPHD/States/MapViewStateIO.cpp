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
#include "../StructureManager.h"
#include "../Map/TileMap.h"
#include "../XmlSerializer.h"

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

	resources.resources[0] = std::stoi(element->attribute(constants::SaveGameResource0));
	resources.resources[1] = std::stoi(element->attribute(constants::SaveGameResource1));
	resources.resources[2] = std::stoi(element->attribute(constants::SaveGameResource2));
	resources.resources[3] = std::stoi(element->attribute(constants::SaveGameResource3));
}


static void readRccRobots(NAS2D::Xml::XmlAttribute* attr, Structure& structure, RobotPool& pool)
{
	if (!attr) { return; }

	for (const auto& string : NAS2D::split(attr->value(), ','))
	{
		const auto robotId = NAS2D::stringTo<int>(string);
		for (auto* robot : pool.robots())
		{
			if (robot->id() == robotId)
			{
				static_cast<RobotCommand*>(&structure)->addRobot(robot);
				break;
			}
		}
	}
}



/*****************************************************************************
 * CLASS FUNCTIONS
 *****************************************************************************/

void MapViewState::save(const std::string& filePath)
{
	auto& renderer = Utility<Renderer>::get();
	renderer.drawBoxFilled(NAS2D::Rectangle{0, 0, renderer.size().x, renderer.size().y}, NAS2D::Color{0, 0, 0, 100});
	const auto imageSaving = &imageCache.load("sys/saving.png");
	renderer.drawImage(*imageSaving, renderer.center() - imageSaving->size() / 2);
	renderer.update();

	XmlDocument doc;

	XmlElement* root = new XmlElement(constants::SaveGameRootNode);
	root->attribute("version", constants::SaveGameVersion);
	doc.linkEndChild(root);

	serializeProperties(root);
	mTileMap->serialize(root);
	Utility<StructureManager>::get().serialize(root);
	writeRobots(root, mRobotPool, mRobotList);
	writeResources(root, mResourceBreakdownPanel.previousResources(), "prev_resources");

	XmlElement* turns = new XmlElement("turns");
	turns->attribute("count", mTurnCount);
	root->linkEndChild(turns);

	XmlElement* population = new XmlElement("population");
	population->attribute("morale", mCurrentMorale);
	population->attribute("prev_morale", mPreviousMorale);
	population->attribute("colonist_landers", mLandersColonist);
	population->attribute("cargo_landers", mLandersCargo);
	population->attribute("children", mPopulation.size(PopulationTable::PersonRole::ROLE_CHILD));
	population->attribute("students", mPopulation.size(PopulationTable::PersonRole::ROLE_STUDENT));
	population->attribute("workers", mPopulation.size(PopulationTable::PersonRole::ROLE_WORKER));
	population->attribute("scientists", mPopulation.size(PopulationTable::PersonRole::ROLE_SCIENTIST));
	population->attribute("retired", mPopulation.size(PopulationTable::PersonRole::ROLE_RETIRED));
	population->attribute("mean_crime", mPopulationPanel.crimeRate());
	root->linkEndChild(population);

	auto moraleChangeReasons = new XmlElement("morale_change");
	auto& moraleChangeList = mPopulationPanel.moraleReasonList();
	for (auto& reason : moraleChangeList)
	{
		auto reasonElement = new XmlElement("change");
		reasonElement->attribute("message", reason.first);
		reasonElement->attribute("val", reason.second);
		moraleChangeReasons->linkEndChild(reasonElement);
	}
	root->linkEndChild(moraleChangeReasons);

	// Write out the XML file.
	XmlMemoryBuffer buff;
	doc.accept(&buff);

	Utility<Filesystem>::get().write(File(buff.buffer(), filePath));
}


void MapViewState::serializeProperties(NAS2D::Xml::XmlElement* element)
{
	XmlElement* properties = new XmlElement("properties");
	element->linkEndChild(properties);

	properties->attribute("sitemap", mPlanetAttributes.mapImagePath);
	properties->attribute("tset", mPlanetAttributes.tilesetPath);
	properties->attribute("diggingdepth", mPlanetAttributes.maxDepth);
	// NAS2D only supports double for floating point conversions as of 26July2020
	properties->attribute("meansolardistance", static_cast<double>(mPlanetAttributes.meanSolarDistance));
	auto diffString = difficultyString(difficulty());
	properties->attribute("difficulty", diffString);
}


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
	mPopulationPanel.clearMoraleReasons();


	if (!Utility<Filesystem>::get().exists(filePath))
	{
		throw std::runtime_error("File '" + filePath + "' was not found.");
	}

	scrubRobotList();
	Utility<StructureManager>::get().dropAllStructures();
	ccLocation() = CcNotPlaced;

	delete mTileMap;
	mTileMap = nullptr;

	auto xmlDocument = openSavegame(filePath);
	auto* root = xmlDocument.firstChildElement(constants::SaveGameRootNode);

	XmlElement* map = root->firstChildElement("properties");
	XmlAttribute* attribute = map->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == "diggingdepth") { attribute->queryIntValue(mPlanetAttributes.maxDepth); }
		else if (attribute->name() == "sitemap") { mPlanetAttributes.mapImagePath = attribute->value(); }
		else if (attribute->name() == "tset") { mPlanetAttributes.tilesetPath = attribute->value(); }
		else if (attribute->name() == "meansolardistance") { mPlanetAttributes.meanSolarDistance = std::stof(attribute->value()); }
		else if (attribute->name() == "difficulty") { difficulty(stringToEnum(difficultyTable, attribute->value())); }
		attribute = attribute->next();
	}

	StructureCatalogue::init(mPlanetAttributes.meanSolarDistance);
	mMapDisplay = std::make_unique<Image>(mPlanetAttributes.mapImagePath + MAP_DISPLAY_EXTENSION);
	mHeightMap = std::make_unique<Image>(mPlanetAttributes.mapImagePath + MAP_TERRAIN_EXTENSION);
	mTileMap = new TileMap(mPlanetAttributes.mapImagePath, mPlanetAttributes.tilesetPath, mPlanetAttributes.maxDepth, 0, Planet::Hostility::None, false);
	mTileMap->deserialize(root);

	delete mPathSolver;
	mPathSolver = new micropather::MicroPather(mTileMap);
	auto& routeTable = NAS2D::Utility<std::map<class MineFacility*, Route>>::get();
	routeTable.clear();

	/**
	 * In the case of loading a game, the Robot Command Center depends on the robot list
	 * having already been loaded in order to match up the robots in the save game to
	 * the RCC.
	 */
	readRobots(root->firstChildElement("robots"));
	readStructures(root->firstChildElement("structures"));

	readResources(root->firstChildElement("prev_resources"), mResourceBreakdownPanel.previousResources());
	readPopulation(root->firstChildElement("population"));
	readTurns(root->firstChildElement("turns"));

	readMoraleChanges(root->firstChildElement("morale_change"));

	checkConnectedness();

	Utility<StructureManager>::get().updateEnergyProduction();
	Utility<StructureManager>::get().updateEnergyConsumed();
	Utility<StructureManager>::get().assignColonistsToResidences(mPopulationPool);

	updateRobotControl(mRobotPool);
	updateResidentialCapacity();
	updateStructuresAvailability();

	findMineRoutes();
	countFood();
	countPlayerResources();

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
			const auto& list = Utility<StructureManager>::get().getStructures<SeedLander>();
			if (list.size() != 1) { throw std::runtime_error("MapViewState::load(): Turn counter at 0 but more than one structure in list."); }

			SeedLander* seedLander = list[0];
			if (!seedLander) { throw std::runtime_error("MapViewState::load(): Structure in list is not a SeedLander."); }

			seedLander->deploySignal().connect(this, &MapViewState::onDeploySeedLander);

			mStructures.clear();
			mConnections.clear();
			mBtnTurns.enabled(true);
		}
	}
	else
	{
		populateStructureMenu();
	}

	CURRENT_LEVEL_STRING = LEVEL_STRING_TABLE[mTileMap->currentDepth()];

	checkCommRangeOverlay();
	checkSurfacePoliceOverlay();

	mMapChangedSignal();
}


void MapViewState::readRobots(Xml::XmlElement* element)
{
	mRobotPool.clear();
	mRobotList.clear();
	mRobots.clear();

	ROBOT_ID_COUNTER = 0;
	for (XmlNode* robotNode = element->firstChild(); robotNode; robotNode = robotNode->nextSibling())
	{
		int id = 0, type = 0, age = 0, production_time = 0, x = 0, y = 0, depth = 0, direction = 0;
		auto* attribute = robotNode->toElement()->firstAttribute();
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
		ROBOT_ID_COUNTER = std::max(ROBOT_ID_COUNTER, id);

		Robot* robot = nullptr;
		switch (static_cast<Robot::Type>(type))
		{
		case Robot::Type::Digger:
			robot = mRobotPool.addRobot(Robot::Type::Digger, id);
			robot->taskComplete().connect(this, &MapViewState::onDiggerTaskComplete);
			static_cast<Robodigger*>(robot)->direction(static_cast<Direction>(direction));
			break;

		case Robot::Type::Dozer:
			robot = mRobotPool.addRobot(Robot::Type::Dozer, id);
			robot->taskComplete().connect(this, &MapViewState::onDozerTaskComplete);
			break;

		case Robot::Type::Miner:
			robot = mRobotPool.addRobot(Robot::Type::Miner, id);
			robot->taskComplete().connect(this, &MapViewState::onMinerTaskComplete);
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

	if (mRobotPool.robotAvailable(Robot::Type::Digger)) { checkRobotSelectionInterface(Robot::Type::Digger); }
	if (mRobotPool.robotAvailable(Robot::Type::Dozer)) { checkRobotSelectionInterface(Robot::Type::Dozer); }
	if (mRobotPool.robotAvailable(Robot::Type::Miner)) { checkRobotSelectionInterface(Robot::Type::Miner); }
}


void MapViewState::readStructures(Xml::XmlElement* element)
{
	for (XmlNode* structureNode = element->firstChild(); structureNode != nullptr; structureNode = structureNode->nextSibling())
	{
		int x = 0, y = 0, depth = 0, age = 0, state = 0, direction = 0, forced_idle = 0, disabled_reason = 0, idle_reason = 0, pop0 = 0, pop1 = 0, type = 0;
		int production_completed = 0, production_type = 0;
		int crime_rate = 0, integrity = 0;
		auto* attribute = structureNode->toElement()->firstAttribute();
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
			else if (attribute->name() == "crime_rate") { attribute->queryIntValue(crime_rate); }
			else if (attribute->name() == "integrity") { attribute->queryIntValue(integrity); }

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
			mineFacility.extensionComplete().connect(this, &MapViewState::onMineFacilityExtend);

			auto trucks = structureNode->firstChildElement("trucks");
			if (trucks)
			{
				auto trucksAssigned = trucks->attribute("assigned");
				mineFacility.assignedTrucks(std::stoi(trucksAssigned));
			}

			auto extension = structureNode->firstChildElement("extension");
			if (extension)
			{
				auto turnsRemaining = extension->attribute("turns_remaining");
				mineFacility.digTimeRemaining(std::stoi(turnsRemaining));
			}
		}

		if (structureId == StructureID::SID_AIR_SHAFT && depth != 0)
		{
			static_cast<AirShaft*>(&structure)->ug(); // force underground state
		}

		if (structureId == StructureID::SID_SEED_LANDER)
		{
			static_cast<SeedLander*>(&structure)->position({x, y});
		}

		if (structureId == StructureID::SID_AGRIDOME ||
			structureId == StructureID::SID_COMMAND_CENTER)
		{
			auto& foodProduction = *static_cast<FoodProduction*>(&structure);

			auto foodStorage = structureNode->firstChildElement("food");
			if (foodStorage == nullptr)
			{
				throw std::runtime_error("MapViewState::readStructures(): FoodProduction structure saved without a food level node.");
			}

			auto foodLevel = foodStorage->attribute("level");
			foodProduction.foodLevel(std::stoi(foodLevel));
		}

		structure.age(age);
		structure.forced_state_change(static_cast<StructureState>(state), static_cast<DisabledReason>(disabled_reason), static_cast<IdleReason>(idle_reason));
		structure.connectorDirection(static_cast<ConnectorDir>(direction));
		structure.integrity(integrity);

		if (forced_idle != 0) { structure.forceIdle(forced_idle != 0); }

		loadResorucesFromXmlElement(structureNode->firstChildElement("production"), structure.production());
		loadResorucesFromXmlElement(structureNode->firstChildElement("storage"), structure.storage());

		if (structure.structureClass() == Structure::StructureClass::Residence)
		{
			auto waste = structureNode->firstChildElement("waste");
			if (waste)
			{
				Residence* residence = static_cast<Residence*>(&structure);
				auto accumulated = waste->attribute("accumulated");
				residence->wasteAccumulated(std::stoi(accumulated));

				auto overflow = waste->attribute("overflow");
				residence->wasteOverflow(std::stoi(overflow));
			}
		}

		if (structure.structureClass() == Structure::StructureClass::Maintenance)
		{
			auto personnel = structureNode->firstChildElement("personnel");
			if (personnel)
			{
				auto maintenanceFacility = static_cast<MaintenanceFacility*>(&structure);
				auto assigned = personnel->attribute("assigned");
				if (!assigned.empty())
				{
					maintenanceFacility->personnel(std::stoi(assigned));
				}

				maintenanceFacility->resources(mResourcesCount);
			}
		}

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
			factory.resourcePool(&mResourcesCount);
			factory.productionComplete().connect(this, &MapViewState::onFactoryProductionComplete);
		}

		if (structure.isRobotCommand())
		{
			auto robotsElement = structureNode->firstChildElement("robots");
			if (robotsElement)
			{
				readRccRobots(robotsElement->firstAttribute(), structure, mRobotPool);
			}
		}

		if (structure.hasCrime())
		{
			structure.crimeRate(crime_rate);
		}

		structure.populationAvailable()[0] = pop0;
		structure.populationAvailable()[1] = pop1;

		Utility<StructureManager>::get().addStructure(&structure, &tile);
	}
}


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
			if (attribute->name() == "morale")
			{
				attribute->queryIntValue(mCurrentMorale);
				mPopulationPanel.morale(mCurrentMorale);
			}
			else if (attribute->name() == "prev_morale")
			{
				attribute->queryIntValue(mPreviousMorale);
				mPopulationPanel.old_morale(mPreviousMorale);
			}
			else if (attribute->name() == "mean_crime")
			{
				int meanCrimeRate = 0;
				attribute->queryIntValue(meanCrimeRate);
				mPopulationPanel.crimeRate(meanCrimeRate);
			}
			else if (attribute->name() == "colonist_landers") { attribute->queryIntValue(mLandersColonist); }
			else if (attribute->name() == "cargo_landers") { attribute->queryIntValue(mLandersCargo); }

			else if (attribute->name() == "children") { attribute->queryIntValue(children); }
			else if (attribute->name() == "students") { attribute->queryIntValue(students); }
			else if (attribute->name() == "workers") { attribute->queryIntValue(workers); }
			else if (attribute->name() == "scientists") { attribute->queryIntValue(scientists); }
			else if (attribute->name() == "retired") { attribute->queryIntValue(retired); }

			attribute = attribute->next();
		}

		mPopulation.addPopulation(PopulationTable::PersonRole::ROLE_CHILD, children);
		mPopulation.addPopulation(PopulationTable::PersonRole::ROLE_STUDENT, students);
		mPopulation.addPopulation(PopulationTable::PersonRole::ROLE_WORKER, workers);
		mPopulation.addPopulation(PopulationTable::PersonRole::ROLE_SCIENTIST, scientists);
		mPopulation.addPopulation(PopulationTable::PersonRole::ROLE_RETIRED, retired);
	}
}


void MapViewState::readMoraleChanges(Xml::XmlElement* elem)
{
	if (!elem) { return; }

	for (auto node = elem->firstChild(); node; node = node->nextSibling())
	{
		auto attribute = node->toElement()->firstAttribute();
		std::string message; int val = 0;
		while (attribute)
		{
			if (attribute->name() == "message") { message = attribute->value(); }
			else if (attribute->name() == "val") { attribute->queryIntValue(val); }
			attribute = attribute->next();
		}
		mPopulationPanel.addMoraleReason(message, val);
	}
}
