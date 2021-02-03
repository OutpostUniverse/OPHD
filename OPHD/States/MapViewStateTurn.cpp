// ==================================================================================
// = This file implements the functions that handle processing a turn.
// ==================================================================================

#include "MapViewState.h"
#include "MapViewStateHelper.h"

#include "../Map/TileMap.h"
#include "../Things/Structures/Structures.h"

#include "../Cache.h"
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

	StructureList &foodproducers = structureManager.structureList(Structure::StructureClass::FoodProduction);
	int remainder = mPopulation.update(mCurrentMorale, mFood, residences, universities, nurseries, hospitals);

	for (auto structure : foodproducers)
	{
		FoodProduction* foodProducer = static_cast<FoodProduction*>(structure);
		pullFoodFromStructure(foodProducer, remainder);
	}

	auto cc = static_cast<FoodProduction*>(mTileMap->getTile(ccLocation(), 0).structure());
	pullFoodFromStructure(cc, remainder);
}


void MapViewState::updateCommercial()
{
	StructureManager& structureManager = NAS2D::Utility<StructureManager>::get();

	StructureList& _warehouses = structureManager.structureList(Structure::StructureClass::Warehouse);
	StructureList& _commercial = structureManager.structureList(Structure::StructureClass::Commercial);

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
	mCurrentMorale += mPopulation.birthCount();
	mCurrentMorale += structureManager.getCountInState(Structure::StructureClass::Park, StructureState::Operational);
	mCurrentMorale += structureManager.getCountInState(Structure::StructureClass::RecreationCenter, StructureState::Operational);

	int food_production = structureManager.getCountInState(Structure::StructureClass::FoodProduction, StructureState::Operational);
	mCurrentMorale += food_production > 0 ? food_production : -5;

	mCurrentMorale += structureManager.getCountInState(Structure::StructureClass::Commercial, StructureState::Operational);

	// NEGATIVE MORALE EFFECTS
	// =========================================
	mCurrentMorale -= mPopulation.deathCount();
	mCurrentMorale -= structureManager.disabled();
	mCurrentMorale -= structureManager.destroyed();

	int residentialMoraleHit = mPopulationPanel.capacity() / 100;

	// Ensure that there is always a morale hit if residential capacity is more than 100%.
	if (mPopulationPanel.capacity() > 100 && residentialMoraleHit < constants::MINIMUM_RESIDENCE_OVERCAPACITY_HIT) { residentialMoraleHit = constants::MINIMUM_RESIDENCE_OVERCAPACITY_HIT; }

	mCurrentMorale -= residentialMoraleHit;

	mCurrentMorale = std::clamp(mCurrentMorale, 0, 1000);
}


void MapViewState::updateResources()
{
	StructureList smelterList = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Smelter);
	auto& routeTable = NAS2D::Utility<std::map<class MineFacility*, Route>>::get();

	mPathSolver->Reset();

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
		}

		/* Route table may have changed, ensure we have a valid iterator. */
		routeIt = routeTable.find(facility);
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

	// Move refined resources from smelters to storage tanks
	for (auto smelter : smelterList)
	{
		if (!smelter->operational() && !smelter->isIdle()) { continue; } // consider a different control path.

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

	updatePlayerResources();
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

	mPopulationPanel.residential_capacity(mResidentialCapacity);
}


void MapViewState::updateBiowasteRecycling()
{
	auto& residences = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Residence);
	auto& recyclingFacilities = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Recycling);

	if (residences.empty() || recyclingFacilities.empty()) { return; }

	auto residenceIterator = residences.begin();
	for (auto recyclingFacility : recyclingFacilities)
	{
		int count = 0;

		// Consider a different control path
		if (!recyclingFacility->operational()) { continue; }

		for (; residenceIterator != residences.end(); ++residenceIterator)
		{
			Residence* residence = static_cast<Residence*>(*residenceIterator);
			residence->pullWaste(30);
			++count;
			if (count >= 10) { break; }
		}
	}
}


void MapViewState::updateFood()
{
	mFood = 0;

	const auto& structures = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::FoodProduction);

	for (auto structure : structures)
	{
		if (structure->operational() || structure->isIdle())
		{
			mFood += static_cast<FoodProduction*>(structure)->foodLevel();
		}
	}

	mFood += static_cast<CommandCenter*>(mTileMap->getTile(ccLocation(), 0).structure())->foodLevel();
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

	mResourceBreakdownPanel.previousResources(mPlayerResources);

	NAS2D::Utility<StructureManager>::get().disconnectAll();
	checkConnectedness();
	NAS2D::Utility<StructureManager>::get().update(mPlayerResources, mPopulationPool);

	updateFood();

	mPreviousMorale = mCurrentMorale;

	updateResidentialCapacity();

	updatePopulation();
	updateCommercial();
	updateBiowasteRecycling();
	updateMorale();
	updateRobots();
	updateResources();
	updateStructuresAvailability();
	updateRoads();

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

	mTurnCount++;
}
