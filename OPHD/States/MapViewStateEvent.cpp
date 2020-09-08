// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ==================================================================================
// = This file implements the non-UI event handlers like factory production, robot
// = task completeion, etc.
// ==================================================================================
#include "MapViewState.h"

#include "../DirectionOffset.h"
#include "../StructureCatalogue.h"
#include "../StructureManager.h"
#include "../Map/TileMap.h"
#include "../Things/Robots/Robots.h"
#include "../Things/Structures/Structures.h"

#include <NAS2D/Utility.h>


void MapViewState::pullRobotFromFactory(ProductType pt, Factory& factory)
{
	RobotCommand* _rc = getAvailableRobotCommand();

	if ((_rc != nullptr) || mRobotPool.commandCapacityAvailable())
	{
		Robot* robot = nullptr;
		
		switch (pt)
		{
		case ProductType::PRODUCT_DIGGER:
			robot = mRobotPool.addRobot(RobotType::ROBOT_DIGGER);
			robot->taskComplete().connect(this, &MapViewState::diggerTaskFinished);
			factory.pullProduct();
			checkRobotSelectionInterface(constants::ROBODIGGER, constants::ROBODIGGER_SHEET_ID, RobotType::ROBOT_DIGGER);
			break;

		case ProductType::PRODUCT_DOZER:
			robot = mRobotPool.addRobot(RobotType::ROBOT_DOZER);
			robot->taskComplete().connect(this, &MapViewState::dozerTaskFinished);
			factory.pullProduct();
			checkRobotSelectionInterface(constants::ROBODOZER, constants::ROBODOZER_SHEET_ID, RobotType::ROBOT_DOZER);
			break;

		case ProductType::PRODUCT_MINER:
			robot = mRobotPool.addRobot(RobotType::ROBOT_MINER);
			robot->taskComplete().connect(this, &MapViewState::minerTaskFinished);
			factory.pullProduct();
			checkRobotSelectionInterface(constants::ROBOMINER, constants::ROBOMINER_SHEET_ID, RobotType::ROBOT_MINER);
			break;

		default:
			throw std::runtime_error("pullRobotFromFactory():: unsuitable robot type.");
		}

		if (_rc != nullptr) { _rc->addRobot(robot); }
	}
	else
	{
		factory.idle(IdleReason::FactoryInsufficientRobotCommandCapacity);
	}

}


/**
 * Called whenever a Factory's production is complete.
 */
void MapViewState::factoryProductionComplete(Factory& factory)
{
	switch (factory.productWaiting())
	{
	case ProductType::PRODUCT_DIGGER:
		pullRobotFromFactory(ProductType::PRODUCT_DIGGER, factory);
		break;

	case ProductType::PRODUCT_DOZER:
		pullRobotFromFactory(ProductType::PRODUCT_DOZER, factory);
		break;

	case ProductType::PRODUCT_MINER:
		pullRobotFromFactory(ProductType::PRODUCT_MINER, factory);
		break;

	case ProductType::PRODUCT_ROAD_MATERIALS:
	case ProductType::PRODUCT_CLOTHING:
	case ProductType::PRODUCT_MEDICINE:
		{
			Warehouse* _wh = getAvailableWarehouse(factory.productWaiting(), 1);
			if (_wh) { _wh->products().store(factory.productWaiting(), 1); factory.pullProduct(); }
			else { factory.idle(IdleReason::FactoryInsufficientWarehouseSpace); }
			break;
		}

	default:
		std::cout << "Unknown Product." << std::endl;
		break;
	}
}


/**
 * Lands colonists on the surfaces and adds them to the population pool.
 */
void MapViewState::deployColonistLander()
{
	mPopulation.addPopulation(Population::PersonRole::ROLE_STUDENT, 10);
	mPopulation.addPopulation(Population::PersonRole::ROLE_WORKER, 20);
	mPopulation.addPopulation(Population::PersonRole::ROLE_SCIENTIST, 20);
}


/**
 * Lands cargo on the surface and adds resources to the resource pool.
 */
void MapViewState::deployCargoLander()
{
	auto cc = static_cast<CommandCenter*>(mTileMap->getTile(ccLocation()).structure());
	cc->foodLevel(cc->foodLevel() + 125);
	cc->storage() += StorableResources{ 25, 25, 15, 15 };

	updateStructuresAvailability();
}


/**
 * Sets up the initial colony deployment.
 *
 * \note	The deploy callback only gets called once so there is really no
 *			need to disconnect the callback since it will automatically be
 *			released when the seed lander is destroyed.
 */
void MapViewState::deploySeedLander(NAS2D::Point<int> point)
{
	// Bulldoze lander region
	for (const auto& direction : DirectionScan3x3)
	{
		mTileMap->getTile(point + direction).index(TerrainType::Dozed);
	}

	auto& structureManager = NAS2D::Utility<StructureManager>::get();

	// Place initial tubes
	for (const auto& direction : DirectionClockwise4)
	{
		structureManager.addStructure(new Tube(ConnectorDir::CONNECTOR_INTERSECTION, false), &mTileMap->getTile(point + direction));
	}

	// TOP ROW
	structureManager.addStructure(new SeedPower(), &mTileMap->getTile(point + DirectionNorthWest));

	CommandCenter* cc = static_cast<CommandCenter*>(StructureCatalogue::get(StructureID::SID_COMMAND_CENTER));
	cc->sprite().setFrame(3);
	structureManager.addStructure(cc, &mTileMap->getTile(point + DirectionNorthEast));
	ccLocation() = point + DirectionNorthEast;

	// BOTTOM ROW
	SeedFactory* sf = static_cast<SeedFactory*>(StructureCatalogue::get(StructureID::SID_SEED_FACTORY));
	sf->resourcePool(&mPlayerResources);
	sf->productionComplete().connect(this, &MapViewState::factoryProductionComplete);
	sf->sprite().setFrame(7);
	structureManager.addStructure(sf, &mTileMap->getTile(point + DirectionSouthWest));

	SeedSmelter* ss = static_cast<SeedSmelter*>(StructureCatalogue::get(StructureID::SID_SEED_SMELTER));
	ss->sprite().setFrame(10);
	structureManager.addStructure(ss, &mTileMap->getTile(point + DirectionSouthEast));

	// Robots only become available after the SEED Factory is deployed.
	mRobots.addItem(constants::ROBODOZER, constants::ROBODOZER_SHEET_ID, RobotType::ROBOT_DOZER);
	mRobots.addItem(constants::ROBODIGGER, constants::ROBODIGGER_SHEET_ID, RobotType::ROBOT_DIGGER);
	mRobots.addItem(constants::ROBOMINER, constants::ROBOMINER_SHEET_ID, RobotType::ROBOT_MINER);
	mRobots.sort();

	mRobotPool.addRobot(RobotType::ROBOT_DOZER)->taskComplete().connect(this, &MapViewState::dozerTaskFinished);
	mRobotPool.addRobot(RobotType::ROBOT_DIGGER)->taskComplete().connect(this, &MapViewState::diggerTaskFinished);
	mRobotPool.addRobot(RobotType::ROBOT_MINER)->taskComplete().connect(this, &MapViewState::minerTaskFinished);
}


/**
 * Called whenever a RoboDozer completes its task.
 */
void MapViewState::dozerTaskFinished(Robot* /*robot*/)
{
	checkRobotSelectionInterface(constants::ROBODOZER, constants::ROBODOZER_SHEET_ID, RobotType::ROBOT_DOZER);
}


/**
 * Called whenever a RoboDigger completes its task.
 */
void MapViewState::diggerTaskFinished(Robot* robot)
{
	if (mRobotList.find(robot) == mRobotList.end()) { throw std::runtime_error("MapViewState::diggerTaskFinished() called with a Robot not in the Robot List!"); }

	Tile* t = mRobotList[robot];

	if (t->depth() > mTileMap->maxDepth())
	{
		throw std::runtime_error("Digger defines a depth that exceeds the maximum digging depth!");
	}

	Direction dir = static_cast<Robodigger*>(robot)->direction(); // fugly

	NAS2D::Point<int> origin = t->position();
	int newDepth = t->depth();

	if(dir == Direction::Down)
	{
		++newDepth;

		AirShaft* as1 = new AirShaft();
		if (t->depth() > 0) { as1->ug(); }
		NAS2D::Utility<StructureManager>::get().addStructure(as1, t);

		AirShaft* as2 = new AirShaft();
		as2->ug();
		NAS2D::Utility<StructureManager>::get().addStructure(as2, &mTileMap->getTile(origin, newDepth));

		mTileMap->getTile(origin, t->depth()).index(TerrainType::Dozed);
		mTileMap->getTile(origin, newDepth).index(TerrainType::Dozed);

		/// \fixme Naive approach; will be slow with large colonies.
		NAS2D::Utility<StructureManager>::get().disconnectAll();
		checkConnectedness();
	}
	else if(dir == Direction::North)
	{
		origin += DirectionNorth;
	}
	else if(dir == Direction::South)
	{
		origin += DirectionSouth;
	}
	else if(dir == Direction::West)
	{
		origin += DirectionWest;
	}
	else if(dir == Direction::East)
	{
		origin += DirectionEast;
	}

	/**
	 * \todo	Add checks for obstructions and things that explode if
	 *			a digger gets in the way (or should diggers be smarter than
	 *			puncturing a fusion reactor containment vessel?)
	 */
	for (const auto offset : DirectionScan3x3)
	{
		mTileMap->getTile(origin + offset, newDepth).excavated(true);
	}

	checkRobotSelectionInterface(constants::ROBODIGGER, constants::ROBODIGGER_SHEET_ID, RobotType::ROBOT_DIGGER);
}


/**
 * Called whenever a RoboMiner completes its task.
 */
void MapViewState::minerTaskFinished(Robot* robot)
{
	if (mRobotList.find(robot) == mRobotList.end()) { throw std::runtime_error("MapViewState::minerTaskFinished() called with a Robot not in the Robot List!"); }

	auto& robotTile = *mRobotList[robot];

	// Surface structure
	MineFacility* mineFacility = new MineFacility(robotTile.mine());
	mineFacility->maxDepth(mTileMap->maxDepth());
	NAS2D::Utility<StructureManager>::get().addStructure(mineFacility, &robotTile);
	mineFacility->extensionComplete().connect(this, &MapViewState::mineFacilityExtended);

	// Tile immediately underneath facility.
	auto& tileBelow = mTileMap->getTile(robotTile.position(), robotTile.depth() + 1);
	NAS2D::Utility<StructureManager>::get().addStructure(new MineShaft(), &tileBelow);

	robotTile.index(TerrainType::Dozed);
	tileBelow.index(TerrainType::Dozed);
	tileBelow.excavated(true);

	robot->die();
}


void MapViewState::mineFacilityExtended(MineFacility* mineFacility)
{
	if (mMineOperationsWindow.mineFacility() == mineFacility) { mMineOperationsWindow.mineFacility(mineFacility); }
	
	auto& mineFacilityTile = NAS2D::Utility<StructureManager>::get().tileFromStructure(mineFacility);
	auto& mineDepthTile = mTileMap->getTile(mineFacilityTile.position(), mineFacility->mine()->depth());
	NAS2D::Utility<StructureManager>::get().addStructure(new MineShaft(), &mineDepthTile);
	mineDepthTile.index(TerrainType::Dozed);
	mineDepthTile.excavated(true);
}
