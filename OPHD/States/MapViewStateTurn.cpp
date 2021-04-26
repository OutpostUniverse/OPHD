// ==================================================================================
// = This file implements the functions that handle processing a turn.
// ==================================================================================

#include "MapViewState.h"
#include "MapViewStateHelper.h"

#include "../Map/TileMap.h"
#include "../Things/Structures/Structures.h"

#include "../Cache.h"
#include "../Common.h"
#include "../DirectionOffset.h"
#include "../StorableResources.h"
#include "../StructureManager.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <vector>
#include <algorithm>


static inline void pullFoodFromStructure(FoodProduction* producer, int& remainder)
{
	if (remainder <= 0) { return; }

	int foodLevel = producer->foodLevel();
	int pulled = pullResource(foodLevel, remainder);

	producer->foodLevel(foodLevel);
	remainder -= pulled;
}


static RouteList findRoutes(micropather::MicroPather* solver, TileMap* tilemap, Structure* mine, const StructureList& smelters)
{
	auto& structureManager = NAS2D::Utility<StructureManager>::get();

	auto& start = structureManager.tileFromStructure(mine);

	RouteList routeList;

	for (auto smelter : smelters)
	{
		auto& end = structureManager.tileFromStructure(smelter);
		tilemap->pathStartAndEnd(&start, &end);
		Route route;
		solver->Solve(&start, &end, &route.path, &route.cost);

		if (!route.empty()) { routeList.push_back(route); }
	}

	return routeList;
}


static Route findLowestCostRoute(RouteList& routeList)
{
	if (routeList.empty()) { return Route(); }

	std::sort(routeList.begin(), routeList.end(), [](const Route& a, const Route& b) { return a.cost < b.cost; });
	return routeList.front();
}


static bool routeObstructed(Route& route)
{
	for (auto tile : route.path)
	{
		Tile* t = static_cast<Tile*>(tile);

		// \note	Tile being occupied by a robot is not an obstruction for the
		//			purposes of routing/pathing.
		if (t->thingIsStructure() && !t->structure()->isRoad()) { return true; }
		if (t->index() == TerrainType::Impassable) { return true; }
	}

	return false;
}


void MapViewState::updatePopulation()
{
	StructureManager& structureManager = NAS2D::Utility<StructureManager>::get();

	int residences = structureManager.getCountInState(Structure::StructureClass::Residence, StructureState::Operational);
	int universities = structureManager.getCountInState(Structure::StructureClass::University, StructureState::Operational);
	int nurseries = structureManager.getCountInState(Structure::StructureClass::Nursery, StructureState::Operational);
	int hospitals = structureManager.getCountInState(Structure::StructureClass::MedicalCenter, StructureState::Operational);

	auto foodproducers = structureManager.structureList(Structure::StructureClass::FoodProduction);
	auto& command = structureManager.structureList(Structure::StructureClass::Command);
	foodproducers.insert(foodproducers.end(), command.begin(), command.end());

	int remainder = mPopulation.update(mCurrentMorale, mFood, residences, universities, nurseries, hospitals);

	for (auto structure : foodproducers)
	{
		FoodProduction* foodProducer = static_cast<FoodProduction*>(structure);
		pullFoodFromStructure(foodProducer, remainder);
	}
}


void MapViewState::updateCommercial()
{
	StructureManager& structureManager = NAS2D::Utility<StructureManager>::get();

	const auto& _warehouses = structureManager.structureList(Structure::StructureClass::Warehouse);
	const auto& _commercial = structureManager.structureList(Structure::StructureClass::Commercial);

	// No need to do anything if there are no commercial structures.
	if (_commercial.empty()) { return; }

	int luxuryCount = structureManager.getCountInState(Structure::StructureClass::Commercial, StructureState::Operational);
	int commercialCount = luxuryCount;

	for (auto warehouse : _warehouses)
	{
		ProductPool& _pl = static_cast<Warehouse*>(warehouse)->products();

		/**
		 * inspect for luxury products.
		 * 
		 * \fixme	I feel like this could be done better. At the moment there
		 *			is only one luxury item, clothing, but as this changes more
		 *			items may be seen as luxury.
		 */
		int clothing = _pl.count(ProductType::PRODUCT_CLOTHING);

		if (clothing >= luxuryCount)
		{
			_pl.pull(ProductType::PRODUCT_CLOTHING, luxuryCount);
			luxuryCount = 0;
			break;
		}
		else if (clothing < luxuryCount)
		{
			_pl.pull(ProductType::PRODUCT_CLOTHING, clothing);
			luxuryCount -= clothing;
		}

		if (luxuryCount == 0)
		{
			break;
		}
	}

	auto _comm_r_it = _commercial.rbegin();
	for (std::size_t i = 0; i < static_cast<std::size_t>(luxuryCount) && _comm_r_it != _commercial.rend(); ++i, ++_comm_r_it)
	{
		if ((*_comm_r_it)->operational())
		{
			(*_comm_r_it)->idle(IdleReason::InsufficientLuxuryProduct);
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
	const int residentialOverCapacityHit = mPopulation.size() > mResidentialCapacity ? 2 : 0;
	const int foodProductionHit = foodProducingStructures > 0 ? 0 : 5;

	auto& residences = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Residence);
	int bioWasteAccumulation = 0;
	for (auto residence : residences)
	{
		Residence* unit = static_cast<Residence*>(residence);
		if (unit->wasteOverflow() > 0) { ++bioWasteAccumulation; }
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
}


void MapViewState::findMineRoutes()
{
	auto& smelterList = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Smelter);
	auto& routeTable = NAS2D::Utility<std::map<class MineFacility*, Route>>::get();
	mPathSolver->Reset();
	mTruckRouteOverlay.clear();

	for (auto mine : NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Mine))
	{
		MineFacility* facility = static_cast<MineFacility*>(mine);
		facility->mine()->checkExhausted();

		if (!mine->operational() && !mine->isIdle()) { continue; } // consider a different control path.

		auto routeIt = routeTable.find(facility);
		bool findNewRoute = routeIt == routeTable.end();

		if (!findNewRoute && routeObstructed(routeIt->second))
		{
			routeTable.erase(facility);
			findNewRoute = true;
		}

		if (findNewRoute)
		{
			auto routeList = findRoutes(mPathSolver, mTileMap, mine, smelterList);
			auto newRoute = findLowestCostRoute(routeList);

			if (newRoute.empty()) { continue; } // give up and move on to the next mine

			routeTable[facility] = newRoute;

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
	for (auto mine : NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Mine))
	{
		auto routeIt = routeTable.find(static_cast<MineFacility*>(mine));
		if (routeIt != routeTable.end())
		{
			const auto& route = routeIt->second;
			const auto smelter = static_cast<Smelter*>(static_cast<Tile*>(route.path.back())->structure());
			const auto mineFacility = static_cast<MineFacility*>(static_cast<Tile*>(route.path.front())->structure());

			if (!smelter->operational()) { break; }

			/* clamp route cost to minimum of 1.0f for next computation to avoid
			   unintended multiplication. */
			const float routeCost = std::clamp(routeIt->second.cost, 1.0f, FLT_MAX);

			/* intentional truncation of fractional component*/
			const int totalOreMovement = static_cast<int>(constants::ShortestPathTraversalCount / routeCost) * mineFacility->assignedTrucks();
			const int oreMovementPart = totalOreMovement / 4;
			const int oreMovementRemainder = totalOreMovement % 4;

			auto& stored = mineFacility->storage();
			StorableResources moved
			{
				std::clamp(stored.resources[0], 0, oreMovementPart),
				std::clamp(stored.resources[1], 0, oreMovementPart),
				std::clamp(stored.resources[2], 0, oreMovementPart),
				std::clamp(stored.resources[3], 0, oreMovementPart + oreMovementRemainder)
			};

			stored -= moved;

			auto& smelterProduction = smelter->production();
			auto newResources = smelterProduction + moved;
			auto capped = newResources.cap(250);
			smelterProduction = capped;

			auto overflow = newResources - capped;
			stored += overflow;
		}
	}
}


void MapViewState::transportResourcesToStorage()
{
	auto& smelterList = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Smelter);
	for (auto smelter : smelterList)
	{
		if (!smelter->operational() && !smelter->isIdle()) { continue; }

		auto& stored = smelter->storage();
		StorableResources moved
		{
			std::clamp(stored.resources[0], 0, 25),
			std::clamp(stored.resources[1], 0, 25),
			std::clamp(stored.resources[2], 0, 25),
			std::clamp(stored.resources[3], 0, 25)
		};

		stored -= moved;
		addRefinedResources(moved);
		stored += moved;
	}
}


void MapViewState::updateResources()
{
	findMineRoutes();
	transportOreFromMines();
	transportResourcesToStorage();
	countPlayerResources();
}


/**
 * Check for colony ship deorbiting; if any colonists are remaining, kill
 * them and reduce morale by an appropriate amount.
 */
void MapViewState::checkColonyShip()
{
	if (mTurnCount == constants::COLONY_SHIP_ORBIT_TIME)
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


void MapViewState::updateResidentialCapacity()
{
	mResidentialCapacity = 0;
	const auto& residences = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Residence);
	for (auto residence : residences)
	{
		if (residence->operational()) { mResidentialCapacity += static_cast<Residence*>(residence)->capacity(); }
	}

	if (residences.empty()) { mResidentialCapacity = constants::COMMAND_CENTER_POPULATION_CAPACITY; }

	mPopulationPanel.residentialCapacity(mResidentialCapacity);
}


void MapViewState::updateBiowasteRecycling()
{
	auto& residences = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Residence);
	auto& recyclingFacilities = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Recycling);

	if (residences.empty() || recyclingFacilities.empty()) { return; }

	auto residenceIterator = residences.begin();
	for (auto recyclingFacility : recyclingFacilities)
	{
		if (!recyclingFacility->operational()) { continue; } // Consider a different control structure

		Recycling* recycling = static_cast<Recycling*>(recyclingFacility);
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


void MapViewState::countFood()
{
	mFood = 0;

	auto structures = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::FoodProduction);
	auto& command = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Command);

	structures.insert(structures.begin(), command.begin(), command.end());

	for (auto structure : structures)
	{
		if (structure->operational() || structure->isIdle())
		{
			mFood += static_cast<FoodProduction*>(structure)->foodLevel();
		}
	}
}


void MapViewState::transferFoodToCommandCenter()
{
	auto& foodProducers = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::FoodProduction);
	auto& command = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Command);

	auto foodProducerIterator = foodProducers.begin();
	for (auto cc : command)
	{
		if (!cc->operational()) { continue; }

		CommandCenter* commandCenter = static_cast<CommandCenter*>(cc);
		int foodToMove = commandCenter->foodCapacity() - commandCenter->foodLevel();

		while (foodProducerIterator != foodProducers.end())
		{
			auto foodProducer = static_cast<FoodProduction*>(*foodProducerIterator);
			const int foodMoved = std::clamp(foodToMove, 0, foodProducer->foodLevel());
			foodProducer->foodLevel(foodProducer->foodLevel() - foodMoved);
			commandCenter->foodLevel(commandCenter->foodLevel() + foodMoved);

			foodToMove -= foodMoved;

			if (foodToMove == 0) { return; }
			
			++foodProducerIterator;
		}
		
	}
}


/**
 * Update road intersection patterns
 */
void MapViewState::updateRoads()
{
	auto roads = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Road);

	for (auto road : roads)
	{
		if (!road->operational()) { continue; }

		const auto tileLocation = NAS2D::Utility<StructureManager>::get().tileFromStructure(road).position();

		std::array<bool, 4> surroundingTiles{ false, false, false, false };
		for (size_t i = 0; i < 4; ++i)
		{
			const auto tileToInspect = tileLocation + DirectionClockwise4[i];
			if (!mTileMap->isValidPosition(tileToInspect)) { continue; }
			if (!mTileMap->getTile(tileToInspect).thingIsStructure()) { continue; }

			surroundingTiles[i] = mTileMap->getTile(tileToInspect).structure()->structureId() == StructureID::SID_ROAD;
		}

		road->sprite().play(IntersectionPatternTable.at(surroundingTiles));
	}
}


void MapViewState::nextTurn()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto imageProcessingTurn = &imageCache.load("sys/processing_turn.png");
	renderer.drawImage(*imageProcessingTurn, renderer.center() - imageProcessingTurn->size() / 2);
	renderer.update();

	clearMode();

	mPopulationPool.clear();

	mResourceBreakdownPanel.previousResources(mResourcesCount);

	NAS2D::Utility<StructureManager>::get().disconnectAll();
	checkConnectedness();
	NAS2D::Utility<StructureManager>::get().update(mResourcesCount, mPopulationPool);

	mPreviousMorale = mCurrentMorale;

	transferFoodToCommandCenter();
	updateResidentialCapacity();

	countFood();
	updatePopulation();

	updateCommercial();
	updateBiowasteRecycling();
	updateMorale();
	updateRobots();
	updateResources();
	updateStructuresAvailability();
	updateRoads();

	// Overlay Updates
	checkCommRangeOverlay();
	onToggleConnectedness();
	onToggleCommRangeOverlay();
	onToggleRouteOverlay();

	auto& factories = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Factory);
	for (auto factory : factories)
	{
		static_cast<Factory*>(factory)->updateProduction();
	}

	populateStructureMenu();

	checkColonyShip();

	/// \fixme There's probably a cleaner way to do this
	mMineOperationsWindow.updateTruckAvailability();

	// Check for Game Over conditions
	if (mPopulation.size() < 1 && mLandersColonist == 0)
	{
		hideUi();
		mGameOverDialog.show();
	}

	mPopulationPanel.morale(mCurrentMorale);
	mPopulationPanel.old_morale(mPreviousMorale);

	mTurnCount++;
}
