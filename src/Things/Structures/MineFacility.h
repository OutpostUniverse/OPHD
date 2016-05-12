#ifndef __MINE_FACILITY__
#define __MINE_FACILITY__

#include "Structure.h"

#include "../../Mine.h"

class MineFacility: public Structure
{
public:
	MineFacility(Mine* mine):	Structure(constants::MINE_FACILITY, "structures/mine_facility.sprite", STRUCTURE_MINE),
								mMine(mine)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(1200);
		turnsToBuild(2);

		requiresCHAP(false);
		selfSustained(true);

		mProductionPool.capacity(500);
		mStoragePool.capacity(500);

		priority(PRIORITY_LOW);
	}


	~MineFacility()
	{}
	

protected:

	virtual void think()
	{
		incrementAge();

		if (!mMine->active() && state() == Structure::OPERATIONAL)
		{
			idle(true);
		}

		if (idle() && mMine->active())
		{
			if (!mStoragePool.atCapacity())
				idle(false);
		}

		if (mMine->active())
		{
			if (mStoragePool.atCapacity())
			{
				idle(true);
				return;
			}

			mMine->update();

			mProductionPool.pushResource(ResourcePool::RESOURCE_COMMON_METALS_ORE, mMine->commonMetalsRate());
			mProductionPool.pushResource(ResourcePool::RESOURCE_COMMON_MINERALS_ORE, mMine->commonMineralsRate());
			mProductionPool.pushResource(ResourcePool::RESOURCE_RARE_METALS_ORE, mMine->rareMetalsRate());
			mProductionPool.pushResource(ResourcePool::RESOURCE_RARE_MINERALS_ORE, mMine->rareMineralsRate());

			storage().pushResources(mProductionPool);
		}
	}

private:

	virtual void defineResourceOutput()
	{}

	virtual void defineResourceValue()
	{
		// Resource value if demolished.
		mResourceValue.commonMetals(10);
		mResourceValue.rareMetals(2);
	}


	Mine*			mMine;

	ResourcePool	mProductionPool;
};

#endif