// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ==================================================================================
// = This file implements the functions that handle processing a turn.
// ==================================================================================

#include "GameState.h"
#include "GameStateHelper.h"

#include "../Things/Structures/Structures.h"


extern NAS2D::Image* IMG_PROCESSING_TURN;	/// \fixme Find a sane place for this.


/**
 * 
 */
static int pullFood(ResourcePool& _rp, int amount)
{
	if (amount <= _rp.food())
	{
		_rp.food(_rp.food() - amount);
		return amount;
	}
	else
	{
		int ret = _rp.food();
		_rp.food(0);
		return ret;
	}
}


/**
 * 
 */
void GameState::updatePopulation()
{
	int residences = mStructureManager.getCountInState(Structure::CLASS_RESIDENCE, Structure::OPERATIONAL);
	int universities = mStructureManager.getCountInState(Structure::CLASS_UNIVERSITY, Structure::OPERATIONAL);
	int nurseries = mStructureManager.getCountInState(Structure::CLASS_NURSERY, Structure::OPERATIONAL);
	int hospitals = mStructureManager.getCountInState(Structure::CLASS_MEDICAL_CENTER, Structure::OPERATIONAL);

	// FOOD CONSUMPTION
	int food_consumed = mPopulation.update(mCurrentMorale, foodInStorage(), residences, universities, nurseries, hospitals);
	StructureManager::StructureList &foodproducers = mStructureManager.structureList(Structure::CLASS_FOOD_PRODUCTION);
	int remainder = food_consumed;

	if (mPlayerResources.food() > 0)
	{
		remainder -= pullFood(mPlayerResources, remainder);
	}

	for (size_t i = 0; i < foodproducers.size(); ++i)
	{
		if (remainder <= 0) { break; }

		remainder -= pullFood(foodproducers[i]->storage(), remainder);
	}
}


/**
 * 
 */
void GameState::updateCommercial()
{
	// Get list of warehouses and commercial.
	StructureManager::StructureList& _warehouses = mStructureManager.structureList(Structure::CLASS_WAREHOUSE);
	StructureManager::StructureList& _commercial = mStructureManager.structureList(Structure::CLASS_COMMERCIAL);

	// No need to do anything if there are no commercial structures.
	if (_commercial.empty()) { return; }

	int luxuryCount = mStructureManager.getCountInState(Structure::CLASS_COMMERCIAL, Structure::OPERATIONAL);
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
		int clothing = _pl.count(PRODUCT_CLOTHING);

		if (clothing >= luxuryCount)
		{
			_pl.pull(PRODUCT_CLOTHING, luxuryCount);
			luxuryCount = 0;
			break;
		}
		else if (clothing < luxuryCount)
		{
			_pl.pull(PRODUCT_CLOTHING, clothing);
			luxuryCount -= clothing;
		}

		if (luxuryCount == 0)
		{
			break;
		}
	}

	auto _comm_r_it = _commercial.rbegin();
	for (size_t i = 0; i < static_cast<size_t>(luxuryCount) && _comm_r_it != _commercial.rend(); ++i, ++_comm_r_it)
	{
		if ((*_comm_r_it)->operational())
		{
			(*_comm_r_it)->idle();
		}
	}

	mCurrentMorale += commercialCount - luxuryCount;
}


/**
 * 
 */
void GameState::updateMorale()
{
	// POSITIVE MORALE EFFECTS
	// =========================================
	mCurrentMorale += mPopulation.birthCount();
	mCurrentMorale += mStructureManager.getCountInState(Structure::CLASS_PARK, Structure::OPERATIONAL);
	mCurrentMorale += mStructureManager.getCountInState(Structure::CLASS_RECREATION_CENTER, Structure::OPERATIONAL);

	int food_production = mStructureManager.getCountInState(Structure::CLASS_FOOD_PRODUCTION, Structure::OPERATIONAL);
	food_production > 0 ? mCurrentMorale += food_production : mCurrentMorale -= 5;

	mCurrentMorale += mStructureManager.getCountInState(Structure::CLASS_COMMERCIAL, Structure::OPERATIONAL);

	// NEGATIVE MORALE EFFECTS
	// =========================================
	mCurrentMorale -= mPopulation.deathCount();
	mCurrentMorale -= mStructureManager.disabled();
	mCurrentMorale -= mStructureManager.destroyed();

	int residentialMoraleHit = static_cast<int>(mPopulationPanel.capacity() / 100.0f);

	// Ensure that there is always a morale hit if residential capacity is more than 100%.
	if (mPopulationPanel.capacity() > 100.0f && residentialMoraleHit < constants::MINIMUM_RESIDENCE_OVERCAPACITY_HIT) { residentialMoraleHit = constants::MINIMUM_RESIDENCE_OVERCAPACITY_HIT; }

	mCurrentMorale -= residentialMoraleHit;

	mCurrentMorale = clamp(mCurrentMorale, 0, 1000);
}


/**
 * 
 */
void GameState::updateResources()
{
	// Update storage capacity
	mPlayerResources.capacity(totalStorage(mStructureManager.structureList(Structure::CLASS_STORAGE)));

	ResourcePool truck;
	truck.capacity(100);

	auto mines = mStructureManager.structureList(Structure::CLASS_MINE);
	auto smelters = mStructureManager.structureList(Structure::CLASS_SMELTER);

	// Move ore from mines to smelters
	for (auto _mine : mines)
	{
		// consider a different control path.
		if (_mine->disabled() || _mine->destroyed()) { continue; }

		ResourcePool& _rp = _mine->storage();

		truck.commonMetalsOre(_rp.pullResource(ResourcePool::RESOURCE_COMMON_METALS_ORE, 25));
		truck.commonMineralsOre(_rp.pullResource(ResourcePool::RESOURCE_COMMON_MINERALS_ORE, 25));
		truck.rareMetalsOre(_rp.pullResource(ResourcePool::RESOURCE_RARE_METALS_ORE, 25));
		truck.rareMineralsOre(_rp.pullResource(ResourcePool::RESOURCE_RARE_MINERALS_ORE, 25));

		for (auto _smelter : smelters)
		{
			if (_smelter->operational())
			{
				_smelter->production().pushResources(truck);
			}
		}

		if (!truck.empty())
		{
			_mine->storage().pushResources(truck);
		}
	}

	// Move refined resources from smelters to storage tanks
	for (auto _smelter : smelters)
	{
		// consider a different control path.
		if (_smelter->disabled() || _smelter->destroyed()) { continue; }

		ResourcePool& _rp = _smelter->storage();
		truck.commonMetals(_rp.pullResource(ResourcePool::RESOURCE_COMMON_METALS, 25));
		truck.commonMinerals(_rp.pullResource(ResourcePool::RESOURCE_COMMON_MINERALS, 25));
		truck.rareMetals(_rp.pullResource(ResourcePool::RESOURCE_RARE_METALS, 25));
		truck.rareMinerals(_rp.pullResource(ResourcePool::RESOURCE_RARE_MINERALS, 25));

		mPlayerResources.pushResources(truck);

		if (!truck.empty())
		{
			_smelter->storage().pushResources(truck);
			break;	// we're at max capacity in our storage, dump what's left in the smelter it came from and barf.
		}
	}
}


/**
 * Check for colony ship deorbiting; if any colonists are remaining, kill
 * them and reduce morale by an appropriate amount.
 */
void GameState::checkColonyShip()
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
			mAnnouncement.announcement(MajorEventAnnouncement::ANNOUNCEMENT_COLONY_SHIP_CRASH_WITH_COLONISTS);
			mAnnouncement.show();
		}
		else
		{
			mWindowStack.bringToFront(&mAnnouncement);
			mAnnouncement.announcement(MajorEventAnnouncement::ANNOUNCEMENT_COLONY_SHIP_CRASH);
			mAnnouncement.show();
		}
	}
}


/**
 * 
 */
void GameState::updateResidentialCapacity()
{
	mResidentialCapacity = 0;
	auto residences = mStructureManager.structureList(Structure::CLASS_RESIDENCE);
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
void GameState::nextTurn()
{
	Renderer& r = Utility<Renderer>::get();
	r.drawImage(*IMG_PROCESSING_TURN, r.center_x() - (IMG_PROCESSING_TURN->width() / 2), r.center_y() - (IMG_PROCESSING_TURN->height() / 2));
	r.update();

	clearMode();

	mPopulationPool.clear();

	mStructureManager.disconnectAll();
	checkConnectedness();
	mStructureManager.update(mPlayerResources, mPopulationPool);

	mPreviousMorale = mCurrentMorale;

	updateResidentialCapacity();

	updatePopulation();
	updateCommercial();
	updateMorale();
	updateRobots();

	updateResources();

	populateStructureMenu();

	checkColonyShip();

	// Check for Game Over conditions
	if (mPopulation.size() < 1 && mLandersColonist == 0)
	{
		hideUi();
		mGameOverDialog.show();
	}

	mTurnCount++;
}
