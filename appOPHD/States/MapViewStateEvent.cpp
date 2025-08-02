// ==================================================================================
// = This file implements the non-UI event handlers like factory production, robot
// = task completion, etc.
// ==================================================================================
#include "MapViewState.h"

#include "MapViewStateHelper.h"

#include "../StructureCatalog.h"
#include "../StructureManager.h"
#include "../Map/TileMap.h"
#include "../MapObjects/Robots.h"
#include "../MapObjects/Structures/AirShaft.h"
#include "../MapObjects/Structures/CommandCenter.h"
#include "../MapObjects/Structures/Factory.h"
#include "../MapObjects/Structures/MineFacility.h"
#include "../MapObjects/Structures/MineShaft.h"
#include "../MapObjects/Structures/SeedFactory.h"
#include "../MapObjects/Structures/Tube.h"
#include "../MapObjects/Structures/Warehouse.h"

#include <libOPHD/DirectionOffset.h>
#include <libOPHD/EnumConnectorDir.h>
#include <libOPHD/EnumIdleReason.h>
#include <libOPHD/MapObjects/OreDeposit.h>

#include <NAS2D/Utility.h>
#include <NAS2D/StringFrom.h>

#include <stdexcept>
#include <array>


void MapViewState::pullRobotFromFactory(ProductType productType, Factory& factory)
{
	const std::map<ProductType, RobotTypeIndex> ProductTypeToRobotTypeIndex
	{
		{ProductType::PRODUCT_DIGGER, RobotTypeIndex::Digger},
		{ProductType::PRODUCT_DOZER, RobotTypeIndex::Dozer},
		{ProductType::PRODUCT_MINER, RobotTypeIndex::Miner},
	};

	if (ProductTypeToRobotTypeIndex.find(productType) == ProductTypeToRobotTypeIndex.end())
	{
		throw std::runtime_error("pullRobotFromFactory():: unsuitable ProductType: " + std::to_string(static_cast<int>(productType)));
	}

	if (mRobotPool.commandCapacityAvailable())
	{
		addRobot(ProductTypeToRobotTypeIndex.at(productType));
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
				mNotificationArea.push({
					"Warehouses full",
					"A factory has shut down due to lack of available warehouse space.",
					factory.xyz(),
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
	if (mTurnNumberOfLanding > mTurnCount) {
		mTurnNumberOfLanding = mTurnCount;
	}
	mPopulation.addPopulation({0, 10, 20, 20, 0});
}


/**
 * Lands cargo on the surface and adds resources to the resource pool.
 */
void MapViewState::onDeployCargoLander()
{
	auto& cc = firstCc();
	cc.foodLevel(cc.foodLevel() + 125);
	cc.storage() += StorableResources{25, 25, 15, 15};

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
		mTileMap->getTile({point + direction, 0}).bulldoze();
	}

	auto& structureManager = NAS2D::Utility<StructureManager>::get();

	constexpr std::array initialStructures{
		std::tuple{DirectionNorth, StructureID::Tube},
		std::tuple{DirectionSouth, StructureID::Tube},
		std::tuple{DirectionWest, StructureID::Tube},
		std::tuple{DirectionEast, StructureID::Tube},
		std::tuple{DirectionNorthWest, StructureID::SeedPower},
		std::tuple{DirectionNorthEast, StructureID::CommandCenter},
		std::tuple{DirectionSouthWest, StructureID::SeedFactory},
		std::tuple{DirectionSouthEast, StructureID::SeedSmelter},
	};

	for (const auto& [direction, structureId] : initialStructures)
	{
		auto& tile = mTileMap->getTile({point + direction, 0});
		auto& structure = structureManager.create(structureId, tile);

		if (auto* seedFactory = dynamic_cast<SeedFactory*>(&structure))
		{
			seedFactory->resourcePool(&mResourcesCount);
			seedFactory->productionCompleteHandler({this, &MapViewState::onFactoryProductionComplete});
		}
	}

	mRobotPool.addRobot(RobotTypeIndex::Dozer).taskCompleteHandler({this, &MapViewState::onDozerTaskComplete});
	mRobotPool.addRobot(RobotTypeIndex::Digger).taskCompleteHandler({this, &MapViewState::onDiggerTaskComplete});
	mRobotPool.addRobot(RobotTypeIndex::Miner).taskCompleteHandler({this, &MapViewState::onMinerTaskComplete});

	populateRobotMenu();
}


/**
 * Called whenever a RoboDozer completes its task.
 */
void MapViewState::onDozerTaskComplete(Robot& /*robot*/)
{
	populateRobotMenu();
}


/**
 * Called whenever a RoboDigger completes its task.
 */
void MapViewState::onDiggerTaskComplete(Robot& robot)
{
	auto& roboDigger = dynamic_cast<Robodigger&>(robot);
	auto& tileMap = *mTileMap;
	auto& tile = roboDigger.tile();
	const auto& position = tile.xyz();
	const auto direction = roboDigger.direction();
	const auto newPosition = position.translate(direction);

	if (direction == Direction::Down)
	{
		auto& bottomTile = tileMap.getTile(newPosition);
		tile.bulldoze();
		bottomTile.bulldoze();

		auto& structureManager = NAS2D::Utility<StructureManager>::get();
		structureManager.create<AirShaft>(tile);
		structureManager.create<AirShaft>(bottomTile);
	}

	for (const auto& offset : DirectionScan3x3)
	{
		mTileMap->getTile({newPosition.xy + offset, newPosition.z}).excavate();
	}

	if (direction == Direction::Down)
	{
		/// \fixme Naive approach; will be slow with large colonies.
		updateConnectedness();
	}

	populateRobotMenu();
}


/**
 * Called whenever a RoboMiner completes its task.
 */
void MapViewState::onMinerTaskComplete(Robot& robot)
{
	auto& robotTile = robot.tile();
	auto& miner = dynamic_cast<Robominer&>(robot);

	auto& mineFacility = miner.buildMine(*mTileMap, robotTile.xyz());
	mineFacility.extensionCompleteHandler({this, &MapViewState::onMineFacilityExtend});
}


void MapViewState::onRobotSelfDestruct(const Robot& robot)
{
	const auto& position = robot.mapCoordinate();
	mNotificationArea.push({
		"Robot Self-Destructed",
		robot.name() + " self destructed at " + NAS2D::stringFrom(position.xy) + ".",
		position,
		NotificationArea::NotificationType::Critical
	});
}


void MapViewState::onRobotBreakDown(const Robot& robot)
{
	const auto& position = robot.mapCoordinate();
	const auto text = robot.name() + " has broken down at " + NAS2D::stringFrom(position.xy) + ". It will not be able to complete its task and will be removed from your inventory.";
	mNotificationArea.push({
		"Robot Broke Down",
		text,
		position,
		NotificationArea::NotificationType::Critical
	});
}


void MapViewState::onRobotTaskComplete(const Robot& robot)
{
	const auto& position = robot.mapCoordinate();
	mNotificationArea.push({
		"Robot Task Completed",
		robot.name() + " completed its task at " + NAS2D::stringFrom(position.xy) + ".",
		position,
		NotificationArea::NotificationType::Success
	});
}


void MapViewState::onRobotTaskCancel(const Robot& robot)
{
	const auto& position = robot.mapCoordinate();
	mNotificationArea.push({
		"Robot Task Canceled",
		robot.name() + " canceled its task at " + NAS2D::stringFrom(position.xy) + ".",
		position,
		NotificationArea::NotificationType::Information
	});
}




void MapViewState::onMineFacilityExtend(MineFacility* mineFacility)
{
	if (mMineOperationsWindow.mineFacility() == mineFacility) { mMineOperationsWindow.mineFacility(mineFacility); }

	auto& structureManager = NAS2D::Utility<StructureManager>::get();
	auto& mineDepthTile = mTileMap->getTile({mineFacility->xyz().xy, mineFacility->oreDeposit().digDepth()});
	structureManager.create<MineShaft>(mineDepthTile);
	mineDepthTile.bulldoze();
	mineDepthTile.excavate();
}


void MapViewState::onCrimeEvent(std::string title, std::string text, const Structure& structure)
{
	mNotificationArea.push({
		std::move(title),
		std::move(text),
		structure.xyz(),
		NotificationArea::NotificationType::Warning
	});
}
