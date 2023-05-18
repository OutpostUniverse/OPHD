// ==================================================================================
// = This file implements the non-UI event handlers like factory production, robot
// = task completeion, etc.
// ==================================================================================
#include "MapViewState.h"

#include "../DirectionOffset.h"
#include "../StructureCatalogue.h"
#include "../StructureManager.h"
#include "../Map/TileMap.h"
#include "../MapObjects/Robots.h"

#include <NAS2D/Utility.h>

#include <stdexcept>
#include <array>


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

	if (mRobotPool.commandCapacityAvailable())
	{
		addRobot(ProductTypeToRobotType.at(productType));
		factory.pullProduct();

		populateRobotMenu();
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
				mNotificationArea.push({
					"Warehouses full",
					"A factory has shut down due to lack of available warehouse space.",
					factoryPos.xyz(),
					NotificationArea::NotificationType::Warning
				});
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

	constexpr std::array initialStructures{
		std::tuple{DirectionNorthWest, StructureID::SID_SEED_POWER},
		std::tuple{DirectionNorthEast, StructureID::SID_COMMAND_CENTER},
		std::tuple{DirectionSouthWest, StructureID::SID_SEED_FACTORY},
		std::tuple{DirectionSouthEast, StructureID::SID_SEED_SMELTER},
	};

	std::vector<Structure*> structures;
	for (const auto& [direction, structureId] : initialStructures)
	{
		auto* structure = StructureCatalogue::get(structureId);
		structureManager.addStructure(*structure, mTileMap->getTile({point + direction, 0}));
		structures.push_back(structure);
	}

	ccLocation() = point + DirectionNorthEast;

	auto& seedFactory = *static_cast<SeedFactory*>(structures[2]);
	seedFactory.resourcePool(&mResourcesCount);
	seedFactory.productionComplete().connect({this, &MapViewState::onFactoryProductionComplete});

	mRobotPool.addRobot(Robot::Type::Dozer).taskComplete().connect({this, &MapViewState::onDozerTaskComplete});
	mRobotPool.addRobot(Robot::Type::Digger).taskComplete().connect({this, &MapViewState::onDiggerTaskComplete});
	mRobotPool.addRobot(Robot::Type::Miner).taskComplete().connect({this, &MapViewState::onMinerTaskComplete});

	populateRobotMenu();
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
	const auto& position = tile.xyz();

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
	auto& miner = *static_cast<Robominer*>(robot);

	auto& mineFacility = miner.buildMine(*mTileMap, robotTile.xyz());
	mineFacility.extensionComplete().connect({this, &MapViewState::onMineFacilityExtend});
}


void MapViewState::onMineFacilityExtend(MineFacility* mineFacility)
{
	if (mMineOperationsWindow.mineFacility() == mineFacility) { mMineOperationsWindow.mineFacility(mineFacility); }

	auto& structureManager = NAS2D::Utility<StructureManager>::get();
	auto& mineFacilityTile = structureManager.tileFromStructure(mineFacility);
	auto& mineDepthTile = mTileMap->getTile({mineFacilityTile.xy(), mineFacility->mine()->depth()});
	structureManager.addStructure(*new MineShaft(), mineDepthTile);
	mineDepthTile.index(TerrainType::Dozed);
	mineDepthTile.excavated(true);
}
