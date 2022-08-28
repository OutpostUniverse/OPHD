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

#include <NAS2D/Utility.h>

#include <stdexcept>


void MapViewState::pullRobotFromFactory(ProductType productType, Factory& factory)
{
	const std::map<ProductType, Robot::Type> ProductTypeToRobotType
	{
		{ProductType::PRODUCT_DIGGER, Robot::Type::Digger},
		{ProductType::PRODUCT_DOZER, Robot::Type::Dozer},
		{ProductType::PRODUCT_MINER, Robot::Type::Miner},
	};

	if (ProductTypeToRobotType.find(productType) == ProductTypeToRobotType.end())
	{
		throw std::runtime_error("pullRobotFromFactory():: unsuitable ProductType: " + std::to_string(static_cast<int>(productType)));
	}

	const auto robotType = ProductTypeToRobotType.at(productType);
	RobotCommand* robotCommand = getAvailableRobotCommand();

	if ((robotCommand != nullptr) || mRobotPool.commandCapacityAvailable())
	{
		auto& robot = addRobot(robotType);
		factory.pullProduct();

		populateRobotMenu();

		if (robotCommand != nullptr) { robotCommand->addRobot(&robot); }
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
	const auto productType = factory.productWaiting();
	switch (productType)
	{
	case ProductType::PRODUCT_DIGGER:
	case ProductType::PRODUCT_DOZER:
	case ProductType::PRODUCT_MINER:
		pullRobotFromFactory(productType, factory);
		break;

	case ProductType::PRODUCT_TRUCK:
	case ProductType::PRODUCT_CLOTHING:
	case ProductType::PRODUCT_MEDICINE:
		{
			Warehouse* warehouse = getAvailableWarehouse(productType, 1);
			if (warehouse) { warehouse->products().store(productType, 1); factory.pullProduct(); }
			else 
			{
				factory.idle(IdleReason::FactoryInsufficientWarehouseSpace); 
				const auto& factoryPos = NAS2D::Utility<StructureManager>::get().tileFromStructure(&factory);
				mNotificationArea.push({"Warehouses full",
										"A factory has shut down due to lack of available warehouse space.",
										factoryPos.xyz(),
										NotificationArea::NotificationType::Warning});
			}
			break;
		}

	default:
		throw std::runtime_error("Unknown product completed");
	}
}


/**
 * Lands colonists on the surfaces and adds them to the population pool.
 */
void MapViewState::onDeployColonistLander()
{
	mPopulation.addPopulation({0, 10, 20, 20, 0});
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
		mTileMap->getTile({point + direction, 0}).index(TerrainType::Dozed);
	}

	auto& structureManager = NAS2D::Utility<StructureManager>::get();

	// Place initial tubes
	for (const auto& direction : DirectionClockwise4)
	{
		structureManager.addStructure(*new Tube(ConnectorDir::CONNECTOR_INTERSECTION, false), mTileMap->getTile({point + direction, 0}));
	}

	// TOP ROW
	structureManager.addStructure(*new SeedPower(), mTileMap->getTile({point + DirectionNorthWest, 0}));

	auto& cc = *static_cast<CommandCenter*>(StructureCatalogue::get(StructureID::SID_COMMAND_CENTER));
	cc.sprite().setFrame(3);
	structureManager.addStructure(cc, mTileMap->getTile({point + DirectionNorthEast, 0}));
	ccLocation() = point + DirectionNorthEast;

	// BOTTOM ROW
	auto& sf = *static_cast<SeedFactory*>(StructureCatalogue::get(StructureID::SID_SEED_FACTORY));
	sf.resourcePool(&mResourcesCount);
	sf.productionComplete().connect({this, &MapViewState::onFactoryProductionComplete});
	sf.sprite().setFrame(7);
	structureManager.addStructure(sf, mTileMap->getTile({point + DirectionSouthWest, 0}));

	auto& ss = *static_cast<SeedSmelter*>(StructureCatalogue::get(StructureID::SID_SEED_SMELTER));
	ss.sprite().setFrame(10);
	structureManager.addStructure(ss, mTileMap->getTile({point + DirectionSouthEast, 0}));

	// Robots only become available after the SEED Factory is deployed.
	mRobots.addItem({constants::Robodozer, constants::RobodozerSheetId, static_cast<int>(Robot::Type::Dozer)});
	mRobots.addItem({constants::Robodigger, constants::RobodiggerSheetId, static_cast<int>(Robot::Type::Digger)});
	mRobots.addItem({constants::Robominer, constants::RobominerSheetId, static_cast<int>(Robot::Type::Miner)});
	mRobots.sort();

	mRobotPool.addRobot(Robot::Type::Dozer).taskComplete().connect({this, &MapViewState::onDozerTaskComplete});
	mRobotPool.addRobot(Robot::Type::Digger).taskComplete().connect({this, &MapViewState::onDiggerTaskComplete});
	mRobotPool.addRobot(Robot::Type::Miner).taskComplete().connect({this, &MapViewState::onMinerTaskComplete});
}


/**
 * Called whenever a RoboDozer completes its task.
 */
void MapViewState::onDozerTaskComplete(Robot* /*robot*/)
{
	populateRobotMenu();
}


/**
 * Called whenever a RoboDigger completes its task.
 */
void MapViewState::onDiggerTaskComplete(Robot* robot)
{
	if (mRobotList.find(robot) == mRobotList.end())
	{
		throw std::runtime_error("MapViewState::onDiggerTaskComplete() called with a Robot not in the Robot List!");
	}

	auto& tile = *mRobotList[robot];
	const auto position = tile.xyz();

	if (position.z > mTileMap->maxDepth())
	{
		throw std::runtime_error("Digger defines a depth that exceeds the maximum digging depth!");
	}

	const auto dir = static_cast<Robodigger*>(robot)->direction(); // fugly
	auto newPosition = position;

	if (dir == Direction::Down)
	{
		++newPosition.z;

		auto& as1 = *new AirShaft();
		if (position.z > 0) { as1.ug(); }
		NAS2D::Utility<StructureManager>::get().addStructure(as1, tile);

		auto& as2 = *new AirShaft();
		as2.ug();
		NAS2D::Utility<StructureManager>::get().addStructure(as2, mTileMap->getTile(newPosition));

		mTileMap->getTile(position).index(TerrainType::Dozed);
		mTileMap->getTile(newPosition).index(TerrainType::Dozed);

		/// \fixme Naive approach; will be slow with large colonies.
		NAS2D::Utility<StructureManager>::get().disconnectAll();
		updateConnectedness();
	}
	newPosition.xy += directionEnumToOffset(dir);

	/**
	 * \todo	Add checks for obstructions and things that explode if
	 *			a digger gets in the way (or should diggers be smarter than
	 *			puncturing a fusion reactor containment vessel?)
	 */
	for (const auto& offset : DirectionScan3x3)
	{
		mTileMap->getTile({newPosition.xy + offset, newPosition.z}).excavated(true);
	}

	populateRobotMenu();
}


/**
 * Called whenever a RoboMiner completes its task.
 */
void MapViewState::onMinerTaskComplete(Robot* robot)
{
	if (mRobotList.find(robot) == mRobotList.end()) { throw std::runtime_error("MapViewState::onMinerTaskComplete() called with a Robot not in the Robot List!"); }

	auto& robotTile = *mRobotList[robot];

	// Surface structure
	auto& mineFacility = *new MineFacility(robotTile.mine());
	mineFacility.maxDepth(mTileMap->maxDepth());
	NAS2D::Utility<StructureManager>::get().addStructure(mineFacility, robotTile);
	mineFacility.extensionComplete().connect({this, &MapViewState::onMineFacilityExtend});

	// Tile immediately underneath facility.
	auto& tileBelow = mTileMap->getTile({robotTile.xy(), robotTile.depth() + 1});
	NAS2D::Utility<StructureManager>::get().addStructure(*new MineShaft(), tileBelow);

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
	NAS2D::Utility<StructureManager>::get().addStructure(*new MineShaft(), mineDepthTile);
	mineDepthTile.index(TerrainType::Dozed);
	mineDepthTile.excavated(true);
}
