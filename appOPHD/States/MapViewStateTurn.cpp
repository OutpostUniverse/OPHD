// ==================================================================================
// = This file implements the functions that handle processing a turn.
// ==================================================================================

#include "MapViewState.h"
#include "MapViewStateHelper.h"
#include "ColonyShip.h"


#include "../Map/Connections.h"
#include "../Map/Route.h"
#include "../Map/RouteFinder.h"
#include "../Map/TileMap.h"

#include "../MapObjects/StructureState.h"
#include "../MapObjects/Structures/CommandCenter.h"
#include "../MapObjects/Structures/Factory.h"
#include "../MapObjects/Structures/MineFacility.h"
#include "../MapObjects/Structures/MaintenanceFacility.h"
#include "../MapObjects/Structures/OreRefining.h"
#include "../MapObjects/Structures/Recycling.h"
#include "../MapObjects/Structures/Residence.h"
#include "../MapObjects/Structures/Road.h"
#include "../MapObjects/Structures/Warehouse.h"

#include "../Cache.h"
#include "../MoraleString.h"
#include "../StructureManager.h"
#include "../Constants/Numbers.h"
#include "../Constants/Strings.h"

#include <libOPHD/DirectionOffset.h>
#include <libOPHD/EnumDifficulty.h>
#include <libOPHD/EnumMoraleIndex.h>
#include <libOPHD/EnumIdleReason.h>
#include <libOPHD/StorableResources.h>
#include <libOPHD/MapObjects/StructureType.h>
#include <libOPHD/Population/MoraleChangeEntry.h>

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <vector>
#include <algorithm>
#include <cfloat>


namespace
{
	const std::map<std::string, StructureID> StructureIdFromString =
	{
		{"FusionReactor", StructureID::FusionReactor},
		{"SolarPlant", StructureID::SolarPlant}
	};

	// Length of "honeymoon period" of no crime/morale updates after landing, in turns
	const std::map<Difficulty, int> GracePeriod
	{
		{Difficulty::Beginner, 30},
		{Difficulty::Easy, 25},
		{Difficulty::Medium, 20},
		{Difficulty::Hard, 15}
	};

	// Morale loss multiplier on colonist death due to colony ship de-orbit
	const std::map<Difficulty, int> ColonyShipDeorbitMoraleLossMultiplier
	{
		{Difficulty::Beginner, 1},
		{Difficulty::Easy, 3},
		{Difficulty::Medium, 6},
		{Difficulty::Hard, 10}
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
		for (auto* foodProducer : foodProducers)
		{
			if (amountToConsume <= 0) { break; }
			amountToConsume -= consumeFood(*foodProducer, amountToConsume);
		}
	}


	constexpr inline int ColonistsPerLander = 50;
	constexpr inline int CargoMoraleLossPerLander = 25;
}


void MapViewState::updatePopulation()
{
	int residences = mStructureManager.operationalCount(StructureClass::Residence);
	int universities = mStructureManager.operationalCount(StructureClass::University);
	int nurseries = mStructureManager.operationalCount(StructureClass::Nursery);
	int hospitals = mStructureManager.operationalCount(StructureClass::MedicalCenter);

	auto foodProducers = mStructureManager.getStructures<FoodProduction>();
	const auto& commandCenters = mStructureManager.getStructures<CommandCenter>();
	foodProducers.insert(foodProducers.end(), commandCenters.begin(), commandCenters.end());

	int amountToConsume = mPopulationModel.update(mMorale.currentMorale(), mFood, residences, universities, nurseries, hospitals);
	consumeFood(foodProducers, amountToConsume);
}


void MapViewState::updateCommercial()
{
	const auto& warehouses = mStructureManager.getStructures<Warehouse>();
	const auto& commercial = mStructureManager.structureList(StructureClass::Commercial);

	// No need to do anything if there are no commercial structures.
	if (commercial.empty()) { return; }

	int luxuryCount = mStructureManager.operationalCount(StructureClass::Commercial);
	int commercialCount = luxuryCount;

	for (auto* warehouse : warehouses)
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

	mMorale.journalMoraleChange({"Commercial - Luxury", commercialCount - luxuryCount});
}


void MapViewState::updateMorale()
{
	// POSITIVE MORALE EFFECTS
	// =========================================
	const int birthCount = mPopulationModel.birthCount();
	const int parkCount = mStructureManager.operationalCount(StructureClass::Park);
	const int recreationCount = mStructureManager.operationalCount(StructureClass::RecreationCenter);
	const int foodProducingStructures = mStructureManager.operationalCount(StructureClass::FoodProduction);
	const int commercialCount = mStructureManager.operationalCount(StructureClass::Commercial);

	// NEGATIVE MORALE EFFECTS
	// =========================================
	const int deathCount = mPopulationModel.deathCount();
	const int structuresDisabled = mStructureManager.disabledCount();
	const int structuresDestroyed = mStructureManager.destroyedCount();
	const int residentialOverCapacityHit = mPopulationModel.getPopulations().size() > mResidentialCapacity ? 2 : 0;
	const int foodProductionHit = foodProducingStructures > 0 ? 0 : 5;

	const auto& residences = mStructureManager.getStructures<Residence>();
	int bioWasteAccumulation = 0;
	for (const auto* residence : residences)
	{
		if (residence->wasteOverflow() > 0) { ++bioWasteAccumulation; }
	}

	// positive
	mMorale.journalMoraleChange({moraleString(MoraleIndexs::Births), birthCount});
	mMorale.journalMoraleChange({moraleString(MoraleIndexs::Parks), parkCount});
	mMorale.journalMoraleChange({moraleString(MoraleIndexs::Recreation), recreationCount});
	mMorale.journalMoraleChange({moraleString(MoraleIndexs::Commercial), commercialCount});

	// negative
	mMorale.journalMoraleChange({moraleString(MoraleIndexs::Deaths), -deathCount});
	mMorale.journalMoraleChange({moraleString(MoraleIndexs::ResidentialOverflow), -residentialOverCapacityHit});
	mMorale.journalMoraleChange({moraleString(MoraleIndexs::BiowasteOverflow), -bioWasteAccumulation * 2}); // TODO 2 is a magic number
	mMorale.journalMoraleChange({moraleString(MoraleIndexs::StructuresDisabled), -structuresDisabled});
	mMorale.journalMoraleChange({moraleString(MoraleIndexs::StructuresDestroyed), -structuresDestroyed});
	mMorale.journalMoraleChange({"Food Production Issues", -foodProductionHit});


	for (const auto& moraleChangeEntry : mCrimeRateUpdate.moraleChanges())
	{
		mMorale.journalMoraleChange(moraleChangeEntry);
	}

	mPopulationPanel.crimeRate(mCrimeRateUpdate.meanCrimeRate());

	for (const auto& moraleChangeEntry : mCrimeExecution.moraleChanges())
	{
		mMorale.journalMoraleChange(moraleChangeEntry);
	}
}


void MapViewState::notifyBirthsAndDeaths()
{
	const int birthCount = mPopulationModel.birthCount();
	const int deathCount = mPopulationModel.deathCount();

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
	const auto& smelterList = mStructureManager.getStructures<OreRefining>();
	auto& routeTable = NAS2D::Utility<std::map<const MineFacility*, Route>>::get();

	for (const auto* mineFacility : mStructureManager.getStructures<MineFacility>())
	{
		if (!mineFacility->operational() && !mineFacility->isIdle()) { continue; } // consider a different control path.

		auto routeIt = routeTable.find(mineFacility);
		bool findNewRoute = routeIt == routeTable.end();

		if (!findNewRoute && routeObstructed(routeIt->second))
		{
			routeTable.erase(mineFacility);
			findNewRoute = true;
		}

		if (findNewRoute)
		{
			auto newRoute = mPathSolver->findLowestCostRoute(mineFacility, smelterList);

			if (newRoute.isEmpty()) { continue; } // give up and move on to the next mine facility.

			routeTable[mineFacility] = newRoute;
		}
	}
}


void MapViewState::transportOreFromMines()
{
	const auto& routeTable = NAS2D::Utility<std::map<const MineFacility*, Route>>::get();
	for (const auto* mineFacilityPtr : mStructureManager.getStructures<MineFacility>())
	{
		auto routeIt = routeTable.find(mineFacilityPtr);
		if (routeIt != routeTable.end())
		{
			const auto& route = routeIt->second;
			auto& smelter = dynamic_cast<OreRefining&>(*route.path.back()->structure());
			auto& mineFacility = dynamic_cast<MineFacility&>(*route.path.front()->structure());

			if (!smelter.operational()) { break; }

			/* clamp route cost to minimum of 1.0f for next computation to avoid
			   unintended multiplication. */
			const float routeCost = std::clamp(routeIt->second.cost, 1.0f, FLT_MAX);

			/* intentional truncation of fractional component*/
			const int totalOreMovement = static_cast<int>(constants::ShortestPathTraversalCount / routeCost) * mineFacility.assignedTrucks();
			const int oreMovementPart = totalOreMovement / 4;
			const int oreMovementRemainder = totalOreMovement % 4;
			const auto movementCap = StorableResources{oreMovementPart, oreMovementPart, oreMovementPart, oreMovementPart + oreMovementRemainder};

			auto& mineStorage = mineFacility.storage();
			auto& smelterStored = smelter.production();

			const auto oreAvailable = smelterStored + mineStorage.cap(movementCap);
			const auto newSmelterStored = oreAvailable.cap(smelter.type().rawOreStorageCapacity);
			const auto movedOre = newSmelterStored - smelterStored;

			mineStorage -= movedOre;
			smelterStored = newSmelterStored;
		}
	}
}


void MapViewState::transportResourcesToStorage()
{
	const auto& smelterList = mStructureManager.getStructures<OreRefining>();
	for (auto* smelter : smelterList)
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


void MapViewState::checkColonyShip()
{
	if (mColonyShip.crashed())
	{
		const auto& crashedLanders = mColonyShip.crashedLanders();
		onColonyShipCrash(crashedLanders);
		mAnnouncement.onColonyShipCrash(mWindowStack, crashedLanders);
	}
}


void MapViewState::onColonyShipCrash(const ColonyShipLanders& colonyShipLanders)
{
	if(colonyShipLanders.colonist > 0)
	{
		int moraleChange = -1 * colonyShipLanders.colonist * ColonyShipDeorbitMoraleLossMultiplier.at(mDifficulty) * ColonistsPerLander;
		mMorale.journalMoraleChange({moraleString(MoraleIndexs::ColonistLanderLost), moraleChange});
	}

	if (colonyShipLanders.cargo > 0)
	{
		int moraleChange = -1 * colonyShipLanders.cargo * ColonyShipDeorbitMoraleLossMultiplier.at(mDifficulty) * CargoMoraleLossPerLander;
		mMorale.journalMoraleChange({moraleString(MoraleIndexs::CargoLanderLost), moraleChange});
	}
}


void MapViewState::checkWarehouseCapacity()
{
	const auto& warehouses = mStructureManager.getStructures<Warehouse>();

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
	mResidentialCapacity = mStructureManager.totalResidentialCapacity();

	const auto& residences = mStructureManager.getStructures<Residence>();
	if (residences.empty()) { mResidentialCapacity = constants::CommandCenterPopulationCapacity; }

	mPopulationPanel.residentialCapacity(mResidentialCapacity);
}


void MapViewState::updateBiowasteRecycling()
{
	const auto& residences = mStructureManager.getStructures<Residence>();
	int bioWasteProcessingCapacity = mStructureManager.totalBioWasteProcessingCapacity();

	for (auto* residence : residences)
	{
		if (bioWasteProcessingCapacity <= 0) { return; }

		const auto processedWaste = residence->removeWaste(bioWasteProcessingCapacity);
		bioWasteProcessingCapacity -= processedWaste;
	}
}


void MapViewState::updateFood()
{
	mFood = 0;

	auto foodProducers = mStructureManager.getStructures<FoodProduction>();
	const auto& command = mStructureManager.getStructures<CommandCenter>();

	foodProducers.insert(foodProducers.begin(), command.begin(), command.end());

	for (const auto* foodProdcer : foodProducers)
	{
		if (foodProdcer->operational() || foodProdcer->isIdle())
		{
			mFood += foodProdcer->foodLevel();
		}
	}
}


void MapViewState::transferFoodToCommandCenter()
{
	const auto& foodProducers = mStructureManager.getStructures<FoodProduction>();
	const auto& commandCenters = mStructureManager.getStructures<CommandCenter>();

	auto foodProducerIterator = foodProducers.begin();
	for (auto* commandCenter : commandCenters)
	{
		if (!commandCenter->operational()) { continue; }

		int foodToMove = commandCenter->foodStorageCapacity() - commandCenter->foodLevel();

		while (foodProducerIterator != foodProducers.end())
		{
			auto foodProducer = dynamic_cast<FoodProduction*>(*foodProducerIterator);
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
	const auto& roads = mStructureManager.getStructures<Road>();

	for (auto* road : roads)
	{
		road->updateConnections(*mTileMap);
	}
}


void MapViewState::checkAgingStructures()
{
	const auto& structures = mStructureManager.agingStructures();

	for (const auto* structure : structures)
	{
		if (structure->age() == structure->maxAge() - 10)
		{
			mNotificationArea.push({
				"Aging Structure",
				structure->name() + " is getting old. You should replace it soon.",
				structure->xyz(),
				NotificationArea::NotificationType::Warning});
		}
		else if (structure->age() == structure->maxAge() - 5)
		{
			mNotificationArea.push({
				"Aging Structure",
				structure->name() + " is about to collapse. You should replace it right away or consider demolishing it.",
				structure->xyz(),
				NotificationArea::NotificationType::Critical});
		}
	}
}


void MapViewState::checkNewlyBuiltStructures()
{
	const auto& structures = mStructureManager.newlyBuiltStructures();

	for (const auto* structure : structures)
	{
		mNotificationArea.push({
			"Construction Finished",
			structure->name() + " completed construction.",
			structure->xyz(),
			NotificationArea::NotificationType::Success});
	}
}


void MapViewState::updateMaintenance()
{
	auto sortLambda = [](const Structure* lhs, const Structure* rhs) -> bool
	{
		return lhs->integrity() < rhs->integrity();
	};

	auto structures = mStructureManager.allStructures();
	std::sort(structures.begin(), structures.end(), sortLambda);

	const auto& maintenanceFacilities = mStructureManager.getStructures<MaintenanceFacility>();
	for (auto* maintenanceFacility : maintenanceFacilities)
	{
		maintenanceFacility->repairStructures(structures);
	}
}


void MapViewState::updateOverlays()
{
	updateRouteOverlay();
	updateCommRangeOverlay();
	updatePoliceOverlay();

	if (mBtnToggleRouteOverlay.isPressed()) { onToggleRouteOverlay(); }
	if (mBtnToggleConnectedness.isPressed()) { onToggleConnectedness(); }
	if (mBtnToggleCommRangeOverlay.isPressed()) { onToggleCommRangeOverlay(); }
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
			const auto structureId = StructureIdFromString.at(unlock.value);
			mStructureTracker.addUnlockedSurfaceStructure(structureId);
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

	mMorale.closeJournal();

	mMapObjectPicker.clearBuildMode();
	mMapObjectPicker.clearSelections();

	mPopulationPool.clear();

	mResourceBreakdownPanel.previousResources(mResourcesCount);

	updateConnectedness();

	mStructureManager.update(mResourcesCount, mPopulationPool);

	checkAgingStructures();
	checkNewlyBuiltStructures();

	transferFoodToCommandCenter();

	updateResidentialCapacity();

	// Colony will not have morale or crime effects until at least n turns from landing, depending on difficulty
	bool isMoraleEnabled = mTurnCount > mTurnNumberOfLanding + GracePeriod.at(mDifficulty);

	if (isMoraleEnabled)
	{
		mCrimeRateUpdate.update();
		auto structuresCommittingCrimes = mCrimeRateUpdate.structuresCommittingCrimes();
		mCrimeExecution.executeCrimes(structuresCommittingCrimes);
	}

	updateFood();
	updatePopulation();

	updateMaintenance();
	updateCommercial();
	updateBiowasteRecycling();

	if (isMoraleEnabled)
	{
		updateMorale();
	}

	notifyBirthsAndDeaths();

	updateRobots();
	updateResources();
	updateStructuresAvailability();
	updateRoads();

	updateOverlays();

	const auto& factories = mStructureManager.getStructures<Factory>();
	for (auto* factory : factories)
	{
		factory->updateProduction();
	}

	updateResearch();

	if (!mColonyShip.crashed())
	{
		mColonyShip.onTurn();
		checkColonyShip();
	}

	checkWarehouseCapacity();

	mMineOperationsWindow.updateTruckAvailability();

	// Check for Game Over conditions
	if (mPopulationModel.getPopulations().size() <= 0 && mColonyShip.colonistLanders() == 0)
	{
		hideUi();
		mGameOverDialog.show();
	}

	populateRobotMenu();
	populateStructureMenu();

	mMorale.commitMoraleChanges();

	mTurnCount++;

	mResourceInfoBar.ignoreGlow(false);
}
