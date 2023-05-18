// ==================================================================================
// = This file implements the functions that handle processing a turn.
// ==================================================================================

#include "MapViewState.h"
#include "MapViewStateHelper.h"

#include "Route.h"

#include "../Map/TileMap.h"

#include "../Cache.h"
#include "../Common.h"
#include "../DirectionOffset.h"
#include "../StorableResources.h"
#include "../StructureManager.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <vector>
#include <algorithm>


namespace
{
	const std::map<std::string, IconGrid::Item> StructureItemFromString =
	{
		{"SID_FUSION_REACTOR", {constants::FusionReactor, 21, SID_FUSION_REACTOR}},
		{"SID_SOLAR_PLANT", {constants::SolarPlant, 10, StructureID::SID_SOLAR_PLANT}}
	};


	int consumeFood(FoodProduction& producer, int amountToConsume)
	{
		const auto foodLevel = producer.foodLevel();
		const auto toTransfer = std::min(foodLevel, amountToConsume);

		producer.foodLevel(foodLevel - toTransfer);
		return toTransfer;
	}


	void consumeFood(const std::vector<FoodProduction*>& foodProducers, int amountToConsume)
	{
		for (auto foodProducer : foodProducers)
		{
			if (amountToConsume <= 0) { break; }
			amountToConsume -= consumeFood(*foodProducer, amountToConsume);
		}
	}


	RouteList findRoutes(micropather::MicroPather* solver, Structure* mine, const std::vector<OreRefining*>& smelters)
	{
		auto& structureManager = NAS2D::Utility<StructureManager>::get();
		auto& start = structureManager.tileFromStructure(mine);

		RouteList routeList;

		for (auto smelter : smelters)
		{
			if (!smelter->operational()) { continue; }

			auto& end = structureManager.tileFromStructure(smelter);

			Route route;
			solver->Reset();
			solver->Solve(&start, &end, &route.path, &route.cost);

			if (!route.empty()) { routeList.push_back(route); }
		}

		return routeList;
	}


	Route findLowestCostRoute(RouteList& routeList)
	{
		if (routeList.empty()) { return Route(); }

		std::sort(routeList.begin(), routeList.end(), [](const Route& a, const Route& b) { return a.cost < b.cost; });
		return routeList.front();
	}


	bool routeObstructed(Route& route)
	{
		for (auto tileVoidPtr : route.path)
		{
			auto& tile = *static_cast<Tile*>(tileVoidPtr);

			// \note	Tile being occupied by a robot is not an obstruction for the
			//			purposes of routing/pathing.
			if (tile.thingIsStructure() && !tile.structure()->isRoad()) { return true; }
			if (tile.index() == TerrainType::Impassable) { return true; }
		}

		return false;
	}
}


void MapViewState::updatePopulation()
{
	StructureManager& structureManager = NAS2D::Utility<StructureManager>::get();

	int residences = structureManager.getCountInState(Structure::StructureClass::Residence, StructureState::Operational);
	int universities = structureManager.getCountInState(Structure::StructureClass::University, StructureState::Operational);
	int nurseries = structureManager.getCountInState(Structure::StructureClass::Nursery, StructureState::Operational);
	int hospitals = structureManager.getCountInState(Structure::StructureClass::MedicalCenter, StructureState::Operational);

	auto foodProducers = structureManager.getStructures<FoodProduction>();
	auto& commandCenters = structureManager.getStructures<CommandCenter>();
	foodProducers.insert(foodProducers.end(), commandCenters.begin(), commandCenters.end());

	int amountToConsume = mPopulation.update(mCurrentMorale, mFood, residences, universities, nurseries, hospitals);
	consumeFood(foodProducers, amountToConsume);
}


void MapViewState::updateCommercial()
{
	StructureManager& structureManager = NAS2D::Utility<StructureManager>::get();

	const auto& warehouses = structureManager.getStructures<Warehouse>();
	const auto& commercial = structureManager.getStructures<Commercial>();

	// No need to do anything if there are no commercial structures.
	if (commercial.empty()) { return; }

	int luxuryCount = structureManager.getCountInState(Structure::StructureClass::Commercial, StructureState::Operational);
	int commercialCount = luxuryCount;

	for (auto warehouse : warehouses)
	{
		ProductPool& productPool = warehouse->products();

		/**
		 * inspect for luxury products.
		 * 
		 * FIXME: I feel like this could be done better. At the moment there
		 * is only one luxury item, clothing, but as this changes more
		 * items may be seen as luxury.
		 */
		int clothing = productPool.count(ProductType::PRODUCT_CLOTHING);

		if (clothing >= luxuryCount)
		{
			productPool.pull(ProductType::PRODUCT_CLOTHING, luxuryCount);
			luxuryCount = 0;
			break;
		}
		else if (clothing < luxuryCount)
		{
			productPool.pull(ProductType::PRODUCT_CLOTHING, clothing);
			luxuryCount -= clothing;
		}

		if (luxuryCount == 0)
		{
			break;
		}
	}

	auto commercialReverseIterator = commercial.rbegin();
	for (std::size_t i = 0; i < static_cast<std::size_t>(luxuryCount) && commercialReverseIterator != commercial.rend(); ++i, ++commercialReverseIterator)
	{
		if ((*commercialReverseIterator)->operational())
		{
			(*commercialReverseIterator)->idle(IdleReason::InsufficientLuxuryProduct);
		}
	}

	mCurrentMorale += commercialCount - luxuryCount;
}


void MapViewState::updateMorale()
{
	StructureManager& structureManager = NAS2D::Utility<StructureManager>::get();

	// POSITIVE MORALE EFFECTS
	// =========================================
	const int birthCount = mPopulation.birthCount();
	const int parkCount = structureManager.getCountInState(Structure::StructureClass::Park, StructureState::Operational);
	const int recreationCount = structureManager.getCountInState(Structure::StructureClass::RecreationCenter, StructureState::Operational);
	const int foodProducingStructures = structureManager.getCountInState(Structure::StructureClass::FoodProduction, StructureState::Operational);
	const int commercialCount = structureManager.getCountInState(Structure::StructureClass::Commercial, StructureState::Operational);

	// NEGATIVE MORALE EFFECTS
	// =========================================
	const int deathCount = mPopulation.deathCount();
	const int structuresDisabled = structureManager.disabled();
	const int structuresDestroyed = structureManager.destroyed();
	const int residentialOverCapacityHit = mPopulation.getPopulations().size() > mResidentialCapacity ? 2 : 0;
	const int foodProductionHit = foodProducingStructures > 0 ? 0 : 5;

	auto& residences = NAS2D::Utility<StructureManager>::get().getStructures<Residence>();
	int bioWasteAccumulation = 0;
	for (auto residence : residences)
	{
		if (residence->wasteOverflow() > 0) { ++bioWasteAccumulation; }
	}

	// positive
	mCurrentMorale += birthCount;
	mCurrentMorale += parkCount;
	mCurrentMorale += recreationCount;
	mCurrentMorale += commercialCount;

	// negative
	mCurrentMorale -= deathCount;
	mCurrentMorale -= residentialOverCapacityHit;
	mCurrentMorale -= bioWasteAccumulation * 2;
	mCurrentMorale -= structuresDisabled;
	mCurrentMorale -= structuresDestroyed;
	mCurrentMorale -= foodProductionHit;

	mCurrentMorale = std::clamp(mCurrentMorale, 0, 1000);

	mPopulationPanel.clearMoraleReasons();
	mPopulationPanel.addMoraleReason(moraleString(Morale::Births), birthCount);
	mPopulationPanel.addMoraleReason(moraleString(Morale::Deaths), -deathCount);
	mPopulationPanel.addMoraleReason(moraleString(Morale::NoFoodProduction), -foodProductionHit);
	mPopulationPanel.addMoraleReason(moraleString(Morale::Parks), parkCount);
	mPopulationPanel.addMoraleReason(moraleString(Morale::Recreation), recreationCount);
	mPopulationPanel.addMoraleReason(moraleString(Morale::Commercial), commercialCount);
	mPopulationPanel.addMoraleReason(moraleString(Morale::ResidentialOverflow), -residentialOverCapacityHit);
	mPopulationPanel.addMoraleReason(moraleString(Morale::BiowasteOverflow), bioWasteAccumulation * -2);
	mPopulationPanel.addMoraleReason(moraleString(Morale::StructuresDisabled), -structuresDisabled);
	mPopulationPanel.addMoraleReason(moraleString(Morale::StructuresDestroyed), -structuresDestroyed);

	for (const auto& moraleReason : mCrimeRateUpdate.moraleChanges())
	{
		mPopulationPanel.addMoraleReason(moraleReason.first, moraleReason.second);
		mCurrentMorale += moraleReason.second;
	}

	mPopulationPanel.crimeRate(mCrimeRateUpdate.meanCrimeRate());

	for (const auto& moraleReason : mCrimeExecution.moraleChanges())
	{
		mPopulationPanel.addMoraleReason(moraleReason.first, moraleReason.second);
		mCurrentMorale += moraleReason.second;
	}

	// Push notifications
	if (birthCount)
	{
		mNotificationArea.push({
			"Baby Born",
			std::to_string(birthCount) + (birthCount > 1 ? " babies were born." : " baby was born."),
			{{-1, -1}, 0},
			NotificationArea::NotificationType::Information});
	}

	if (deathCount)
	{
		mNotificationArea.push({
			"Colonist Died",
			std::to_string(deathCount) + (birthCount > 1 ? " colonists met their demise." : " colonist met their demise."),
			{{-1, -1}, 0},
			NotificationArea::NotificationType::Warning});
	}
}


void MapViewState::findMineRoutes()
{
	auto& smelterList = NAS2D::Utility<StructureManager>::get().getStructures<OreRefining>();
	auto& routeTable = NAS2D::Utility<std::map<class MineFacility*, Route>>::get();
	mTruckRouteOverlay.clear();

	for (auto mine : NAS2D::Utility<StructureManager>::get().getStructures<MineFacility>())
	{
		if (!mine->operational() && !mine->isIdle()) { continue; } // consider a different control path.

		auto routeIt = routeTable.find(mine);
		bool findNewRoute = routeIt == routeTable.end();

		if (!findNewRoute && routeObstructed(routeIt->second))
		{
			routeTable.erase(mine);
			findNewRoute = true;
		}

		if (findNewRoute)
		{
			auto routeList = findRoutes(mPathSolver, mine, smelterList);
			auto newRoute = findLowestCostRoute(routeList);

			if (newRoute.empty()) { continue; } // give up and move on to the next mine

			routeTable[mine] = newRoute;

			for (auto tile : newRoute.path)
			{
				mTruckRouteOverlay.push_back(static_cast<Tile*>(tile));
			}
		}
	}
}


void MapViewState::transportOreFromMines()
{
	auto& routeTable = NAS2D::Utility<std::map<class MineFacility*, Route>>::get();
	for (auto mine : NAS2D::Utility<StructureManager>::get().getStructures<MineFacility>())
	{
		auto routeIt = routeTable.find(mine);
		if (routeIt != routeTable.end())
		{
			const auto& route = routeIt->second;
			auto& smelter = *static_cast<OreRefining*>(static_cast<Tile*>(route.path.back())->structure());
			auto& mineFacility = *static_cast<MineFacility*>(static_cast<Tile*>(route.path.front())->structure());

			if (!smelter.operational()) { break; }

			/* clamp route cost to minimum of 1.0f for next computation to avoid
			   unintended multiplication. */
			const float routeCost = std::clamp(routeIt->second.cost, 1.0f, FLT_MAX);

			/* intentional truncation of fractional component*/
			const int totalOreMovement = static_cast<int>(constants::ShortestPathTraversalCount / routeCost) * mineFacility.assignedTrucks();
			const int oreMovementPart = totalOreMovement / 4;
			const int oreMovementRemainder = totalOreMovement % 4;
			const auto movementCap = StorableResources{oreMovementPart, oreMovementPart, oreMovementPart, oreMovementPart + oreMovementRemainder};

			auto& mineStored = mineFacility.storage();
			auto& smelterStored = smelter.production();

			const auto oreAvailable = smelterStored + mineStored.cap(movementCap);
			const auto newSmelterStored = oreAvailable.cap(250);
			const auto movedOre = newSmelterStored - smelterStored;

			mineStored -= movedOre;
			smelterStored = newSmelterStored;
		}
	}
}


void MapViewState::transportResourcesToStorage()
{
	auto& smelterList = NAS2D::Utility<StructureManager>::get().getStructures<OreRefining>();
	for (auto smelter : smelterList)
	{
		if (!smelter->operational() && !smelter->isIdle()) { continue; }

		auto& stored = smelter->storage();
		const auto toMove = stored.cap(25);

		const auto unmoved = addRefinedResources(toMove);
		stored -= (toMove - unmoved);
	}
}


void MapViewState::updateResources()
{
	findMineRoutes();
	transportOreFromMines();
	transportResourcesToStorage();
	updatePlayerResources();
}


/**
 * Check for colony ship deorbiting; if any colonists are remaining, kill
 * them and reduce morale by an appropriate amount.
 */
void MapViewState::checkColonyShip()
{
	if (mTurnCount == constants::ColonyShipOrbitTime)
	{
		if (mLandersColonist > 0 || mLandersCargo > 0)
		{
			mCurrentMorale -= (mLandersColonist * 50) * 6; /// \todo apply a modifier to multiplier based on difficulty level.
			if (mCurrentMorale < 0) { mCurrentMorale = 0; }

			mLandersColonist = 0;
			mLandersCargo = 0;

			populateStructureMenu();

			mWindowStack.bringToFront(&mAnnouncement);
			mAnnouncement.announcement(MajorEventAnnouncement::AnnouncementType::ANNOUNCEMENT_COLONY_SHIP_CRASH_WITH_COLONISTS);
			mAnnouncement.show();
		}
		else
		{
			mWindowStack.bringToFront(&mAnnouncement);
			mAnnouncement.announcement(MajorEventAnnouncement::AnnouncementType::ANNOUNCEMENT_COLONY_SHIP_CRASH);
			mAnnouncement.show();
		}
	}
}


void MapViewState::checkWarehouseCapacity()
{
	StructureManager& structureManager = NAS2D::Utility<StructureManager>::get();
	const auto& warehouses = structureManager.getStructures<Warehouse>();

	if (warehouses.size() == 0) { return; } // no divisions by zero, pl0x

	int availableStorageTotal = 0;
	for (const auto warehouse : warehouses)
	{
		availableStorageTotal += warehouse->products().availableStoragePercent();
	}

	const int availableStorage = availableStorageTotal / static_cast<int>(warehouses.size());

	if (availableStorage == 0) // FIXME -- Magic Number
	{
		mNotificationArea.push({
			"No Warehouse Space",
			"You are out of storage space at your warehouses! Your Factories will go idle until you build more Warehouses or reduce inventory.",
			{{-1, -1}, 0},
			NotificationArea::NotificationType::Critical
		});
	}
	else if (availableStorage < 5) // FIXME -- Ditto
	{
		mNotificationArea.push({
			"Warehouse Space Critically Low",
			"Warehouse space is critically low! You only have " + std::to_string(availableStorage) + "% storage capacity remaining!",
			{{-1, -1}, 0},
			NotificationArea::NotificationType::Critical
		});
	}
	else if (availableStorage < 15) // FIXME -- Ditto
	{
		mNotificationArea.push({
			"Warehouse Space Low",
			"Warehouse space is running low. Current available storage capacity is at " + std::to_string(availableStorage) + "%.",
			{{-1, -1}, 0},
			NotificationArea::NotificationType::Warning
		});
	}
}


void MapViewState::updateResidentialCapacity()
{
	mResidentialCapacity = 0;
	const auto& residences = NAS2D::Utility<StructureManager>::get().getStructures<Residence>();
	for (auto residence : residences)
	{
		if (residence->operational()) { mResidentialCapacity += residence->capacity(); }
	}

	if (residences.empty()) { mResidentialCapacity = constants::CommandCenterPopulationCapacity; }

	mPopulationPanel.residentialCapacity(mResidentialCapacity);
}


void MapViewState::updateBiowasteRecycling()
{
	auto& residences = NAS2D::Utility<StructureManager>::get().getStructures<Residence>();
	auto& recyclingFacilities = NAS2D::Utility<StructureManager>::get().getStructures<Recycling>();

	if (residences.empty() || recyclingFacilities.empty()) { return; }

	auto residenceIterator = residences.begin();
	for (auto recycling : recyclingFacilities)
	{
		if (!recycling->operational()) { continue; } // Consider a different control structure

		for (int count = 0; count < recycling->residentialSupportCount(); ++count)
		{
			if (residenceIterator == residences.end())
			{
				return; // No more residences, so don't waste time iterating over remaining recycling facilities
			}

			Residence* residence = static_cast<Residence*>(*residenceIterator);
			residence->pullWaste(recycling->wasteProcessingCapacity());
			++residenceIterator;
		}
	}
}


void MapViewState::updateFood()
{
	mFood = 0;

	auto foodProducers = NAS2D::Utility<StructureManager>::get().getStructures<FoodProduction>();
	auto& command = NAS2D::Utility<StructureManager>::get().getStructures<CommandCenter>();

	foodProducers.insert(foodProducers.begin(), command.begin(), command.end());

	for (auto foodProdcer : foodProducers)
	{
		if (foodProdcer->operational() || foodProdcer->isIdle())
		{
			mFood += foodProdcer->foodLevel();
		}
	}
}


void MapViewState::transferFoodToCommandCenter()
{
	auto& foodProducers = NAS2D::Utility<StructureManager>::get().getStructures<FoodProduction>();
	auto& commandCenters = NAS2D::Utility<StructureManager>::get().getStructures<CommandCenter>();

	auto foodProducerIterator = foodProducers.begin();
	for (auto commandCenter : commandCenters)
	{
		if (!commandCenter->operational()) { continue; }

		int foodToMove = commandCenter->foodCapacity() - commandCenter->foodLevel();

		while (foodProducerIterator != foodProducers.end())
		{
			auto foodProducer = static_cast<FoodProduction*>(*foodProducerIterator);
			const int foodMoved = std::clamp(foodToMove, 0, foodProducer->foodLevel());
			foodProducer->foodLevel(foodProducer->foodLevel() - foodMoved);
			commandCenter->foodLevel(commandCenter->foodLevel() + foodMoved);

			foodToMove -= foodMoved;

			if (foodToMove == 0) { break; }

			++foodProducerIterator;
		}
	}
}


/**
 * Update road intersection patterns
 */
void MapViewState::updateRoads()
{
	auto roads = NAS2D::Utility<StructureManager>::get().getStructures<Road>();

	for (auto road : roads)
	{
		if (!road->operational()) { continue; }

		const auto tileLocation = NAS2D::Utility<StructureManager>::get().tileFromStructure(road).xy();

		std::array<bool, 4> surroundingTiles{false, false, false, false};
		for (size_t i = 0; i < 4; ++i)
		{
			const auto tileToInspect = tileLocation + DirectionClockwise4[i];
			const auto surfacePosition = MapCoordinate{tileToInspect, 0};
			if (!mTileMap->isValidPosition(surfacePosition)) { continue; }
			const auto& tile = mTileMap->getTile(surfacePosition);
			if (!tile.thingIsStructure()) { continue; }

			surroundingTiles[i] = tile.structure()->structureId() == StructureID::SID_ROAD;
		}

		std::string tag = "";
		
		if (road->integrity() < constants::RoadIntegrityChange) { tag = "-decayed"; }
		else if (road->integrity() == 0) { tag = "-destroyed"; }

		road->sprite().play(IntersectionPatternTable.at(surroundingTiles) + tag);
	}
}


void MapViewState::checkAgingStructures()
{
	const auto& structures = NAS2D::Utility<StructureManager>::get().agingStructures();

	for (auto structure : structures)
	{
		const auto& structureTile = NAS2D::Utility<StructureManager>::get().tileFromStructure(structure);

		if (structure->age() == structure->maxAge() - 10)
		{
			mNotificationArea.push({
				"Aging Structure",
				structure->name() + " is getting old. You should replace it soon.",
				structureTile.xyz(),
				NotificationArea::NotificationType::Warning});
		}
		else if (structure->age() == structure->maxAge() - 5)
		{
			mNotificationArea.push({
				"Aging Structure",
				structure->name() + " is about to collapse. You should replace it right away or consider demolishing it.",
				structureTile.xyz(),
				NotificationArea::NotificationType::Critical});
		}
	}
}


void MapViewState::checkNewlyBuiltStructures()
{
	const auto& structures = NAS2D::Utility<StructureManager>::get().newlyBuiltStructures();

	for (auto structure : structures)
	{
		const auto& structureTile = NAS2D::Utility<StructureManager>::get().tileFromStructure(structure);

		mNotificationArea.push({
			"Construction Finished",
			structure->name() + " completed construction.",
			structureTile.xyz(),
			NotificationArea::NotificationType::Success});
	}
}


void MapViewState::updateMaintenance()
{
	auto sortLambda = [](const Structure* lhs, const Structure* rhs) -> bool
	{
		return lhs->integrity() < rhs->integrity();
	};

	auto& structureManager = NAS2D::Utility<StructureManager>::get();
	auto structures = structureManager.allStructures();
	std::sort(structures.begin(), structures.end(), sortLambda);

	auto& maintenanceFacilities = structureManager.getStructures<MaintenanceFacility>();
	for (auto maintenanceFacility : maintenanceFacilities)
	{
		maintenanceFacility->repairStructures(structures);
	}
}


void MapViewState::updateOverlays()
{
	updateCommRangeOverlay();
	updatePoliceOverlay();

	if (mBtnToggleConnectedness.isPressed()) { onToggleConnectedness(); }
	if (mBtnToggleCommRangeOverlay.isPressed()) { onToggleCommRangeOverlay(); }
	if (mBtnToggleRouteOverlay.isPressed()) { onToggleRouteOverlay(); }
	if (mBtnTogglePoliceOverlay.isPressed()) { onTogglePoliceOverlay(); }
}


void MapViewState::updateResearch()
{
	// Update research points
	// get list of completed technologies
	const auto& completedTechs = mResearchTracker.completedResearch();
	std::vector<const Technology*> techList;
	for (const auto techId : completedTechs)
	{
		techList.push_back(&mTechnologyReader.technologyFromId(techId));
	}

	// get list of completed technologies that unlock buildings
	std::vector<const Technology*> unlockedStructures;
	
	for (auto tech : techList)
	{
		for (const auto& unlock : (*tech).unlocks)
		{
			if (unlock.unlocks == Technology::Unlock::Unlocks::Structure)
			{
				unlockedStructures.push_back(tech);
			}
		}
	}
	
	for (const auto& tech : unlockedStructures)
	{
		for (const auto& unlock : tech->unlocks)
		{
			const auto& structureItem = StructureItemFromString.at(unlock.value);
			mStructureTracker.addUnlockedSurfaceStructure(structureItem);
		}
	}
	
	// remove obsolete structures from available structure list
}


void MapViewState::nextTurn()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto imageProcessingTurn = &imageCache.load("sys/processing_turn.png");
	renderer.drawImage(*imageProcessingTurn, renderer.center() - imageProcessingTurn->size() / 2);
	renderer.update();

	mNotificationWindow.hide();
	mNotificationArea.clear();

	clearMode();

	mPopulationPool.clear();

	mResourceBreakdownPanel.previousResources(mResourcesCount);

	updateConnectedness();
	NAS2D::Utility<StructureManager>::get().update(mResourcesCount, mPopulationPool);

	checkAgingStructures();
	checkNewlyBuiltStructures();

	mPreviousMorale = mCurrentMorale;

	transferFoodToCommandCenter();

	updateResidentialCapacity();

	if (mPopulation.getPopulations().size() > 0)
	{
		mCrimeRateUpdate.update(mPoliceOverlays);
		auto structuresCommittingCrimes = mCrimeRateUpdate.structuresCommittingCrimes();
		mCrimeExecution.executeCrimes(structuresCommittingCrimes);
	}

	updateFood();
	updatePopulation();

	updateMaintenance();
	updateCommercial();
	updateBiowasteRecycling();
	updateMorale();
	updateRobots();
	updateResources();
	updateStructuresAvailability();
	updateRoads();

	updateOverlays();

	auto& factories = NAS2D::Utility<StructureManager>::get().getStructures<Factory>();
	for (auto factory : factories)
	{
		factory->updateProduction();
	}

	updateResearch();

	populateRobotMenu();
	populateStructureMenu();

	checkColonyShip();
	checkWarehouseCapacity();

	mMineOperationsWindow.updateTruckAvailability();

	// Check for Game Over conditions
	if (mPopulation.getPopulations().size() <= 0 && mLandersColonist == 0)
	{
		hideUi();
		mGameOverDialog.show();
	}

	mPopulationPanel.morale(mCurrentMorale);
	mPopulationPanel.old_morale(mPreviousMorale);

	mTurnCount++;

	mResourceInfoBar.ignoreGlow(false);
}
