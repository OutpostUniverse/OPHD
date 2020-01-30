// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ==================================================================================
// = This file implements the non-UI event handlers like factory production, robot
// = task completeion, etc.
// ==================================================================================
#include "MapViewState.h"

#include "../StructureCatalogue.h"

#include "../Things/Robots/Robots.h"
#include "../Things/Structures/Structures.h"


void MapViewState::pullRobotFromFactory(ProductType pt, Factory& factory)
{
	RobotCommand* _rc = getAvailableRobotCommand();

	if ((_rc != nullptr) || mRobotPool.commandCapacityAvailable())
	{
		Robot* r = nullptr;
		
		switch (pt)
		{
		case PRODUCT_DIGGER:
			r = mRobotPool.addRobot(ROBOT_DIGGER);
			r->taskComplete().connect(this, &MapViewState::diggerTaskFinished);
			factory.pullProduct();
			checkRobotSelectionInterface(constants::ROBODIGGER, constants::ROBODIGGER_SHEET_ID, ROBOT_DIGGER);
			break;

		case PRODUCT_DOZER:
			r = mRobotPool.addRobot(ROBOT_DOZER);
			r->taskComplete().connect(this, &MapViewState::dozerTaskFinished);
			factory.pullProduct();
			checkRobotSelectionInterface(constants::ROBODOZER, constants::ROBODOZER_SHEET_ID, ROBOT_DOZER);
			break;

		case PRODUCT_MINER:
			r = mRobotPool.addRobot(ROBOT_MINER);
			r->taskComplete().connect(this, &MapViewState::minerTaskFinished);
			factory.pullProduct();
			checkRobotSelectionInterface(constants::ROBOMINER, constants::ROBOMINER_SHEET_ID, ROBOT_MINER);
			break;

		default:
			throw std::runtime_error("pullRobotFromFactory():: unsuitable robot type.");
		}

		if (_rc != nullptr) { _rc->addRobot(r); }
	}
	else
	{
		factory.idle(IDLE_FACTORY_INSUFFICIENT_ROBOT_COMMAND_CAPACITY);
	}

}


/**
 * Called whenever a Factory's production is complete.
 */
void MapViewState::factoryProductionComplete(Factory& factory)
{	
	switch (factory.productWaiting())
	{
	case PRODUCT_DIGGER:
		pullRobotFromFactory(PRODUCT_DIGGER, factory);
		break;

	case PRODUCT_DOZER:
		pullRobotFromFactory(PRODUCT_DOZER, factory);
		break;

	case PRODUCT_MINER:
		pullRobotFromFactory(PRODUCT_MINER, factory);
		break;

	case PRODUCT_ROAD_MATERIALS:
	case PRODUCT_CLOTHING:
	case PRODUCT_MEDICINE:
		{
			Warehouse* _wh = getAvailableWarehouse(factory.productWaiting(), 1);
			if (_wh) { _wh->products().store(factory.productWaiting(), 1); factory.pullProduct(); }
			else { factory.idle(IDLE_FACTORY_INSUFFICIENT_WAREHOUSE_SPACE); }
			break;
		}

	default:
		std::cout << "Unknown Product." << std::endl;
		break;
	}
}


/**
 * Lands colonists on the surfaces and adds them to the population pool.
 */
void MapViewState::deployColonistLander()
{
	mPopulation.addPopulation(Population::ROLE_STUDENT, 10);
	mPopulation.addPopulation(Population::ROLE_WORKER, 20);
	mPopulation.addPopulation(Population::ROLE_SCIENTIST, 20);
}


/**
 * Lands cargo on the surface and adds resources to the resource pool.
 */
void MapViewState::deployCargoLander()
{
	///\fixme Magic numbers
	mPlayerResources.commonMetals(mPlayerResources.commonMetals() + 25);
	mPlayerResources.commonMinerals(mPlayerResources.commonMinerals() + 25);
	mPlayerResources.rareMetals(mPlayerResources.rareMetals() + 15);
	mPlayerResources.rareMinerals(mPlayerResources.rareMinerals() + 15);

	mPlayerResources.food(mPlayerResources.food() + 125);
}


/**
 * Sets up the initial colony deployment.
 *
 * \note	The deploy callback only gets called once so there is really no
 *			need to disconnect the callback since it will automatically be
 *			released when the seed lander is destroyed.
 */
void MapViewState::deploySeedLander(int x, int y)
{
	mTileMap->getTile(x, y)->index(TERRAIN_DOZED);

	// TOP ROW
	Utility<StructureManager>::get().addStructure(new SeedPower(), mTileMap->getTile(x - 1, y - 1));
	mTileMap->getTile(x - 1, y - 1)->index(TERRAIN_DOZED);

	Utility<StructureManager>::get().addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap->getTile(x, y - 1));
	mTileMap->getTile(x, y - 1)->index(TERRAIN_DOZED);

	CommandCenter* cc = static_cast<CommandCenter*>(StructureCatalogue::get(SID_COMMAND_CENTER));
	cc->sprite().skip(3);
	Utility<StructureManager>::get().addStructure(cc, mTileMap->getTile(x + 1, y - 1));
	mTileMap->getTile(x + 1, y - 1)->index(TERRAIN_DOZED);
	ccLocation() = {x + 1, y - 1};

	// MIDDLE ROW
	mTileMap->getTile(x - 1, y)->index(TERRAIN_DOZED);
	Utility<StructureManager>::get().addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap->getTile(x - 1, y));

	mTileMap->getTile(x + 1, y)->index(TERRAIN_DOZED);
	Utility<StructureManager>::get().addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap->getTile(x + 1, y));

	// BOTTOM ROW
	SeedFactory* sf = static_cast<SeedFactory*>(StructureCatalogue::get(SID_SEED_FACTORY));
	sf->resourcePool(&mPlayerResources);
	sf->productionComplete().connect(this, &MapViewState::factoryProductionComplete);
	sf->sprite().skip(7);
	Utility<StructureManager>::get().addStructure(sf, mTileMap->getTile(x - 1, y + 1));
	mTileMap->getTile(x - 1, y + 1)->index(TERRAIN_DOZED);

	mTileMap->getTile(x, y + 1)->index(TERRAIN_DOZED);
	Utility<StructureManager>::get().addStructure(new Tube(CONNECTOR_INTERSECTION, false), mTileMap->getTile(x, y + 1));

	SeedSmelter* ss = static_cast<SeedSmelter*>(StructureCatalogue::get(SID_SEED_SMELTER));
	ss->sprite().skip(10);
	Utility<StructureManager>::get().addStructure(ss, mTileMap->getTile(x + 1, y + 1));
	mTileMap->getTile(x + 1, y + 1)->index(TERRAIN_DOZED);

	// Robots only become available after the SEED Factor is deployed.
	mRobots.addItem(constants::ROBODOZER, constants::ROBODOZER_SHEET_ID, ROBOT_DOZER);
	mRobots.addItem(constants::ROBODIGGER, constants::ROBODIGGER_SHEET_ID, ROBOT_DIGGER);
	mRobots.addItem(constants::ROBOMINER, constants::ROBOMINER_SHEET_ID, ROBOT_MINER);
	mRobots.sort();

	mRobotPool.addRobot(ROBOT_DOZER)->taskComplete().connect(this, &MapViewState::dozerTaskFinished);
	mRobotPool.addRobot(ROBOT_DIGGER)->taskComplete().connect(this, &MapViewState::diggerTaskFinished);
	mRobotPool.addRobot(ROBOT_MINER)->taskComplete().connect(this, &MapViewState::minerTaskFinished);
}


/**
 * Called whenever a RoboDozer completes its task.
 */
void MapViewState::dozerTaskFinished(Robot* /*_r*/)
{
	checkRobotSelectionInterface(constants::ROBODOZER, constants::ROBODOZER_SHEET_ID, ROBOT_DOZER);
}


/**
 * Called whenever a RoboDigger completes its task.
 */
void MapViewState::diggerTaskFinished(Robot* _r)
{
	if (mRobotList.find(_r) == mRobotList.end()) { throw std::runtime_error("MapViewState::diggerTaskFinished() called with a Robot not in the Robot List!"); }

	Tile* t = mRobotList[_r];

	if (t->depth() > mTileMap->maxDepth())
	{
		throw std::runtime_error("Digger defines a depth that exceeds the maximum digging depth!");
	}

	Direction dir = static_cast<Robodigger*>(_r)->direction(); // fugly

	int originX = 0, originY = 0, depthAdjust = 0;

	if(dir == DIR_DOWN)
	{
		AirShaft* as1 = new AirShaft();
		if (t->depth() > 0) { as1->ug(); }
		Utility<StructureManager>::get().addStructure(as1, t);

		AirShaft* as2 = new AirShaft();
		as2->ug();
		Utility<StructureManager>::get().addStructure(as2, mTileMap->getTile(t->x(), t->y(), t->depth() + 1));

		originX = t->x();
		originY = t->y();
		depthAdjust = 1;

		mTileMap->getTile(originX, originY, t->depth())->index(TERRAIN_DOZED);
		mTileMap->getTile(originX, originY, t->depth() + depthAdjust)->index(TERRAIN_DOZED);

		/// \fixme Naive approach; will be slow with large colonies.
		Utility<StructureManager>::get().disconnectAll();
		checkConnectedness();
	}
	else if(dir == DIR_NORTH)
	{
		originX = t->x();
		originY = t->y() - 1;
	}
	else if(dir == DIR_SOUTH)
	{
		originX = t->x();
		originY = t->y() + 1;
	}
	else if(dir == DIR_WEST)
	{
		originX = t->x() - 1;
		originY = t->y();
	}
	else if(dir == DIR_EAST)
	{
		originX = t->x() + 1;
		originY = t->y();
	}

	/**
	 * \todo	Add checks for obstructions and things that explode if
	 *			a digger gets in the way (or should diggers be smarter than
	 *			puncturing a fusion reactor containment vessel?)
	 */
	for(int y = originY - 1; y <= originY + 1; ++y)
	{
		for(int x = originX - 1; x <= originX + 1; ++x)
		{
			mTileMap->getTile(x, y, t->depth() + depthAdjust)->excavated(true);
		}
	}

	checkRobotSelectionInterface(constants::ROBODIGGER, constants::ROBODIGGER_SHEET_ID, ROBOT_DIGGER);
}


/**
 * Called whenever a RoboMiner completes its task.
 */
void MapViewState::minerTaskFinished(Robot* _r)
{
	if (mRobotList.find(_r) == mRobotList.end()) { throw std::runtime_error("MapViewState::minerTaskFinished() called with a Robot not in the Robot List!"); }

	Tile* t = mRobotList[_r];

	// Surface structure
	MineFacility* _mf = new MineFacility(t->mine());
	_mf->maxDepth(mTileMap->maxDepth());
	Utility<StructureManager>::get().addStructure(_mf, t);
	_mf->extensionComplete().connect(this, &MapViewState::mineFacilityExtended);

	// Tile immediately underneath facility.
	Tile* t2 = mTileMap->getTile(t->x(), t->y(), t->depth() + 1);
	Utility<StructureManager>::get().addStructure(new MineShaft(), t2);

	t->index(0);
	t2->index(0);
	t2->excavated(true);

	_r->die();
}


void MapViewState::mineFacilityExtended(MineFacility* mf)
{
	if (mMineOperationsWindow.mineFacility() == mf) { mMineOperationsWindow.mineFacility(mf); }
	
	Tile* mf_tile = Utility<StructureManager>::get().tileFromStructure(mf);
	Tile* t = mTileMap->getTile(mf_tile->x(), mf_tile->y(), mf->mine()->depth());
	Utility<StructureManager>::get().addStructure(new MineShaft(), t);
	t->index(0);
	t->excavated(true);
}
