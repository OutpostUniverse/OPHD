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
	RobotCommand* robotCommand = getAvailableRobotCommand();

	if ((robotCommand != nullptr) || mRobotPool.commandCapacityAvailable())
	{
		Robot* robot = nullptr;

		switch (pt)
		{
		case ProductType::PRODUCT_DIGGER:
			robot = mRobotPool.addRobot(Robot::Type::Digger);
			robot->taskComplete().connect(this, &MapViewState::onDiggerTaskComplete);
			factory.pullProduct();
			checkRobotSelectionInterface(Robot::Type::Digger);
			break;

		case ProductType::PRODUCT_DOZER:
			robot = mRobotPool.addRobot(Robot::Type::Dozer);
			robot->taskComplete().connect(this, &MapViewState::onDozerTaskComplete);
			factory.pullProduct();
			checkRobotSelectionInterface(Robot::Type::Dozer);
			break;

		case ProductType::PRODUCT_MINER:
			robot = mRobotPool.addRobot(Robot::Type::Miner);
			robot->taskComplete().connect(this, &MapViewState::onMinerTaskComplete);
			factory.pullProduct();
			checkRobotSelectionInterface(Robot::Type::Miner);
			break;

		default:
			throw std::runtime_error("pullRobotFromFactory():: unsuitable robot type.");
		}

		if (robotCommand != nullptr) { robotCommand->addRobot(robot); }
	}
	else
	{
		factory.idle(IdleReason::FactoryInsufficientRobotCommandCapacity);
	}

}


/**
 * Called whenever a Factory's production is complete.
 */
void MapViewState::onFactoryProductionComplete(Factory& factory)
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

	case ProductType::PRODUCT_TRUCK:
	case ProductType::PRODUCT_CLOTHING:
	case ProductType::PRODUCT_MEDICINE:
		{
			Warehouse* warehouse = getAvailableWarehouse(factory.productWaiting(), 1);
			if (warehouse) { warehouse->products().store(factory.productWaiting(), 1); factory.pullProduct(); }
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
void MapViewState::onDeployColonistLander()
{
	mPopulation.addPopulation(PopulationTable::Role::Student, 10);
	mPopulation.addPopulation(PopulationTable::Role::Worker, 20);
	mPopulation.addPopulation(PopulationTable::Role::Scientist, 20);
}


/**
 * Lands cargo on the surface and adds resources to the resource pool.
 */
void MapViewState::onDeployCargoLander()
{
	auto cc = static_cast<CommandCenter*>(mTileMap->getTile({ccLocation(), 0}).structure());
	cc->foodLevel(cc->foodLevel() + 125);
	cc->storage() += StorableResources{25, 25, 15, 15};

	updateStructuresAvailability();
}


/**
 * Sets up the initial colony deployment.
 *
 * \note	The deploy callback only gets called once so there is really no
 *			need to disconnect the callback since it will automatically be
 *			released when the seed lander is destroyed.
 */
void MapViewState::onDeploySeedLander(NAS2D::Point<int> point)
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
	sf->resourcePool(&mResourcesCount);
	sf->productionComplete().connect(this, &MapViewState::onFactoryProductionComplete);
	sf->sprite().setFrame(7);
	structureManager.addStructure(sf, &mTileMap->getTile(point + DirectionSouthWest));

	SeedSmelter* ss = static_cast<SeedSmelter*>(StructureCatalogue::get(StructureID::SID_SEED_SMELTER));
	ss->sprite().setFrame(10);
	structureManager.addStructure(ss, &mTileMap->getTile(point + DirectionSouthEast));

	// Robots only become available after the SEED Factory is deployed.
	mRobots.addItem(constants::Robodozer, constants::RobodozerSheetId, static_cast<int>(Robot::Type::Dozer));
	mRobots.addItem(constants::Robodigger, constants::RobodiggerSheetId, static_cast<int>(Robot::Type::Digger));
	mRobots.addItem(constants::Robominer, constants::RobominerSheetId, static_cast<int>(Robot::Type::Miner));
	mRobots.sort();

	mRobotPool.addRobot(Robot::Type::Dozer)->taskComplete().connect(this, &MapViewState::onDozerTaskComplete);
	mRobotPool.addRobot(Robot::Type::Digger)->taskComplete().connect(this, &MapViewState::onDiggerTaskComplete);
	mRobotPool.addRobot(Robot::Type::Miner)->taskComplete().connect(this, &MapViewState::onMinerTaskComplete);
}


/**
 * Called whenever a RoboDozer completes its task.
 */
void MapViewState::onDozerTaskComplete(Robot* /*robot*/)
{
	checkRobotSelectionInterface(Robot::Type::Dozer);
}


/**
 * Called whenever a RoboDigger completes its task.
 */
void MapViewState::onDiggerTaskComplete(Robot* robot)
{
	if (mRobotList.find(robot) == mRobotList.end()) { throw std::runtime_error("MapViewState::onDiggerTaskComplete() called with a Robot not in the Robot List!"); }

	Tile* tile = mRobotList[robot];
	const auto position = tile->xyz();

	if (position.z > mTileMap->maxDepth())
	{
		throw std::runtime_error("Digger defines a depth that exceeds the maximum digging depth!");
	}

	Direction dir = static_cast<Robodigger*>(robot)->direction(); // fugly

	auto newPosition = position;

	if (dir == Direction::Down)
	{
		++newPosition.z;

		AirShaft* as1 = new AirShaft();
		if (position.z > 0) { as1->ug(); }
		NAS2D::Utility<StructureManager>::get().addStructure(as1, tile);

		AirShaft* as2 = new AirShaft();
		as2->ug();
		NAS2D::Utility<StructureManager>::get().addStructure(as2, &mTileMap->getTile(newPosition));

		mTileMap->getTile(position).index(TerrainType::Dozed);
		mTileMap->getTile(newPosition).index(TerrainType::Dozed);

		/// \fixme Naive approach; will be slow with large colonies.
		NAS2D::Utility<StructureManager>::get().disconnectAll();
		checkConnectedness();
	}
	else if (dir == Direction::North)
	{
		newPosition.xy += DirectionNorth;
	}
	else if (dir == Direction::South)
	{
		newPosition.xy += DirectionSouth;
	}
	else if (dir == Direction::West)
	{
		newPosition.xy += DirectionWest;
	}
	else if (dir == Direction::East)
	{
		newPosition.xy += DirectionEast;
	}

	/**
	 * \todo	Add checks for obstructions and things that explode if
	 *			a digger gets in the way (or should diggers be smarter than
	 *			puncturing a fusion reactor containment vessel?)
	 */
	for (const auto& offset : DirectionScan3x3)
	{
		mTileMap->getTile({newPosition.xy + offset, newPosition.z}).excavated(true);
	}

	checkRobotSelectionInterface(Robot::Type::Digger);
}


/**
 * Called whenever a RoboMiner completes its task.
 */
void MapViewState::onMinerTaskComplete(Robot* robot)
{
	if (mRobotList.find(robot) == mRobotList.end()) { throw std::runtime_error("MapViewState::onMinerTaskComplete() called with a Robot not in the Robot List!"); }

	auto& robotTile = *mRobotList[robot];

	// Surface structure
	MineFacility* mineFacility = new MineFacility(robotTile.mine());
	mineFacility->maxDepth(mTileMap->maxDepth());
	NAS2D::Utility<StructureManager>::get().addStructure(mineFacility, &robotTile);
	mineFacility->extensionComplete().connect(this, &MapViewState::onMineFacilityExtend);

	// Tile immediately underneath facility.
	auto& tileBelow = mTileMap->getTile({robotTile.xy(), robotTile.depth() + 1});
	NAS2D::Utility<StructureManager>::get().addStructure(new MineShaft(), &tileBelow);

	robotTile.index(TerrainType::Dozed);
	tileBelow.index(TerrainType::Dozed);
	tileBelow.excavated(true);

	robot->die();
}


void MapViewState::onMineFacilityExtend(MineFacility* mineFacility)
{
	if (mMineOperationsWindow.mineFacility() == mineFacility) { mMineOperationsWindow.mineFacility(mineFacility); }

	auto& mineFacilityTile = NAS2D::Utility<StructureManager>::get().tileFromStructure(mineFacility);
	auto& mineDepthTile = mTileMap->getTile({mineFacilityTile.xy(), mineFacility->mine()->depth()});
	NAS2D::Utility<StructureManager>::get().addStructure(new MineShaft(), &mineDepthTile);
	mineDepthTile.index(TerrainType::Dozed);
	mineDepthTile.excavated(true);
}
