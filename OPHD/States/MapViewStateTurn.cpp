// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ==================================================================================
// = This file implements the functions that handle processing a turn.
// ==================================================================================

#include "MapViewState.h"
#include "MapViewStateHelper.h"

#include "../Things/Structures/Structures.h"

#include <vector>
#include <algorithm>


extern NAS2D::Image* IMG_PROCESSING_TURN; /// \fixme Find a sane place for this.


/**
 * 
 */
static int pullFood(ResourcePool& resourcePool, int amount)
{
	if (amount <= resourcePool.food())
	{
		resourcePool.food(resourcePool.food() - amount);
		return amount;
	}
	else
	{
		int ret = resourcePool.food();
		resourcePool.food(0);
		return ret;
	}
}


/**
 * 
 */
void MapViewState::updatePopulation()
{
	StructureManager& structureManager = NAS2D::Utility<StructureManager>::get();
	
	int residences = structureManager.getCountInState(Structure::StructureClass::Residence, Structure::StructureState::OPERATIONAL);
	int universities = structureManager.getCountInState(Structure::StructureClass::University, Structure::StructureState::OPERATIONAL);
	int nurseries = structureManager.getCountInState(Structure::StructureClass::Nursery, Structure::StructureState::OPERATIONAL);
	int hospitals = structureManager.getCountInState(Structure::StructureClass::MedicalCenter, Structure::StructureState::OPERATIONAL);

	// FOOD CONSUMPTION
	int food_consumed = mPopulation.update(mCurrentMorale, foodInStorage(), residences, universities, nurseries, hospitals);
	StructureList &foodproducers = structureManager.structureList(Structure::StructureClass::FoodProduction);
	int remainder = food_consumed;

	if (mPlayerResources.food() > 0)
	{
		remainder -= pullFood(mPlayerResources, remainder);
	}

	for (std::size_t i = 0; i < foodproducers.size(); ++i)
	{
		if (remainder <= 0) { break; }

		remainder -= pullFood(foodproducers[i]->storage(), remainder);
	}
}


/**
 * 
 */
void MapViewState::updateCommercial()
{
	StructureManager& structureManager = NAS2D::Utility<StructureManager>::get();

	StructureList& _warehouses = structureManager.structureList(Structure::StructureClass::Warehouse);
	StructureList& _commercial = structureManager.structureList(Structure::StructureClass::Commercial);

	// No need to do anything if there are no commercial structures.
	if (_commercial.empty()) { return; }

	int luxuryCount = structureManager.getCountInState(Structure::StructureClass::Commercial, Structure::StructureState::OPERATIONAL);
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
			(*_comm_r_it)->idle(IdleReason::IDLE_INSUFFICIENT_LUXURY_PRODUCT);
		}
	}

	mCurrentMorale += commercialCount - luxuryCount;
}


/**
 * 
 */
void MapViewState::updateMorale()
{
	StructureManager& structureManager = NAS2D::Utility<StructureManager>::get();

	// POSITIVE MORALE EFFECTS
	// =========================================
	mCurrentMorale += mPopulation.birthCount();
	mCurrentMorale += structureManager.getCountInState(Structure::StructureClass::Park, Structure::StructureState::OPERATIONAL);
	mCurrentMorale += structureManager.getCountInState(Structure::StructureClass::RecreationCenter, Structure::StructureState::OPERATIONAL);

	int food_production = structureManager.getCountInState(Structure::StructureClass::FoodProduction, Structure::StructureState::OPERATIONAL);
	mCurrentMorale += food_production > 0 ? food_production : -5;

	mCurrentMorale += structureManager.getCountInState(Structure::StructureClass::Commercial, Structure::StructureState::OPERATIONAL);

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



static RouteList findRoutes(micropather::MicroPather* solver, TileMap* tilemap, Structure* mine, const StructureList& smelters)
{
	auto& structureManager = NAS2D::Utility<StructureManager>::get();

	Tile* start = structureManager.tileFromStructure(mine);

	RouteList routeList;

	for (auto smelter : smelters)
	{
		Tile* end = structureManager.tileFromStructure(smelter);
		tilemap->pathStartAndEnd(start, end);
		Route route;
		solver->Solve(start, end, &route.path, &route.cost);

		if (!route.empty()) { routeList.push_back(route); }
	}

	return routeList;
}


static bool compareRoute(Route& a, Route& b) { return a.path.size() < b.path.size(); }


static Route findLowestCostRoute(RouteList& routeList)
{
	if (routeList.empty()) { return Route(); }

	std::sort(routeList.begin(), routeList.end(), compareRoute);
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
		if (t->index() == TERRAIN_IMPASSABLE) { return true; }
	}

	return false;
}


/**
 * 
 */
void MapViewState::updateResources()
{
	// Update storage capacity
	mPlayerResources.capacity(totalStorage(NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Storage)));

	ResourcePool truck(100);

	StructureList smelterList = NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Smelter);

	mPathSolver->Reset();

	for (auto mine : NAS2D::Utility<StructureManager>::get().structureList(Structure::StructureClass::Mine))
	{
		MineFacility* facility = static_cast<MineFacility*>(mine);
		facility->mine()->checkExhausted();

		if (!mine->operational() && !mine->isIdle()) { continue; } // consider a different control path.

		auto route = mRouteTable.find(facility);
		bool findNewRoute = route == mRouteTable.end();

		if (!findNewRoute && routeObstructed(route->second))
		{
			mRouteTable.erase(facility);
			findNewRoute = true;
		}

		if (findNewRoute)
		{
			auto routeList = findRoutes(mPathSolver, mTileMap, mine, smelterList);
			auto newRoute = findLowestCostRoute(routeList);

			if (newRoute.empty()) { continue; } // give up and move on to the next mine

			mRouteTable[facility] = newRoute;
		}

		// do resource movement here
	}

	// Move refined resources from smelters to storage tanks
	for (auto smelter : smelterList)
	{
		if (!smelter->operational()) { continue; } // consider a different control path.

		ResourcePool& resourcePool = smelter->storage();
		truck.commonMetals(resourcePool.pullResource(ResourcePool::ResourceType::RESOURCE_COMMON_METALS, 25));
		truck.commonMinerals(resourcePool.pullResource(ResourcePool::ResourceType::RESOURCE_COMMON_MINERALS, 25));
		truck.rareMetals(resourcePool.pullResource(ResourcePool::ResourceType::RESOURCE_RARE_METALS, 25));
		truck.rareMinerals(resourcePool.pullResource(ResourcePool::ResourceType::RESOURCE_RARE_MINERALS, 25));

		mPlayerResources.pushResources(truck);

		if (!truck.empty())
		{
			smelter->storage().pushResources(truck);
			break; // we're at max capacity in our storage, dump what's left in the smelter it came from and barf.
		}
	}
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


/**
 * 
 */
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


/**
 * 
 */
void MapViewState::nextTurn()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	renderer.drawImage(*IMG_PROCESSING_TURN, renderer.center() - IMG_PROCESSING_TURN->size() / 2);
	renderer.update();

	clearMode();

	mPopulationPool.clear();

	mResourceBreakdownPanel.previousResources() = mPlayerResources;

	NAS2D::Utility<StructureManager>::get().disconnectAll();
	checkConnectedness();
	NAS2D::Utility<StructureManager>::get().update(mPlayerResources, mPopulationPool);

	mPreviousMorale = mCurrentMorale;

	updateResidentialCapacity();

	updatePopulation();
	updateCommercial();
	updateMorale();
	updateRobots();

	updateResources();

	mResourceBreakdownPanel.resourceCheck();

	populateStructureMenu();

	checkColonyShip();

	mStructureInspector.check();

	// Check for Game Over conditions
	if (mPopulation.size() < 1 && mLandersColonist == 0)
	{
		hideUi();
		mGameOverDialog.show();
	}

	mTurnCount++;
}
