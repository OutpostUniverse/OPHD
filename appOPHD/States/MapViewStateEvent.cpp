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
#include <libOPHD/EnumIdleReason.h>
#include <libOPHD/MapObjects/OreDeposit.h>

#include <NAS2D/Utility.h>

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

	// Place initial tubes
	for (const auto& direction : DirectionClockwise4)
	{
		auto& tile = mTileMap->getTile({point + direction, 0});
		structureManager.addStructure(*new Tube(tile, ConnectorDir::CONNECTOR_INTERSECTION), tile);
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
		auto& tile = mTileMap->getTile({point + direction, 0});
		auto* structure = StructureCatalog::create(structureId, tile);
		structureManager.addStructure(*structure, tile);
		structures.push_back(structure);
	}

	auto& seedFactory = *dynamic_cast<SeedFactory*>(structures[2]);
	seedFactory.resourcePool(&mResourcesCount);
	seedFactory.productionCompleteHandler({this, &MapViewState::onFactoryProductionComplete});

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

	/**
	 * \todo	Add checks for obstructions and things that explode if
	 *			a digger gets in the way (or should diggers be smarter than
	 *			puncturing a fusion reactor containment vessel?)
	 */
	for (const auto& offset : DirectionScan3x3)
	{
		mTileMap->getTile({newPosition.xy + offset, newPosition.z}).excavated(true);
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


void MapViewState::onMineFacilityExtend(MineFacility* mineFacility)
{
	if (mMineOperationsWindow.mineFacility() == mineFacility) { mMineOperationsWindow.mineFacility(mineFacility); }

	auto& structureManager = NAS2D::Utility<StructureManager>::get();
	auto& mineDepthTile = mTileMap->getTile({mineFacility->xyz().xy, mineFacility->oreDeposit().digDepth()});
	structureManager.create<MineShaft>(mineDepthTile);
	mineDepthTile.bulldoze();
	mineDepthTile.excavated(true);
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
