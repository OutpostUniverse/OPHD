#ifndef __MINE_FACILITY__
#define __MINE_FACILITY__

#include "Structure.h"

#include "../../Mine.h"

class MineFacility: public Structure
{
public:
	MineFacility(Mine* mine):	Structure(constants::MINE_FACILITY, "structures/mine_facility.sprite"),
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

	void update()
	{
		incrementAge();

		if (age() == turnsToBuild())
		{
			sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
			idle(false);
			activate();
			mMine->active(true);
		}

		if(!mMine->active() && state() == Structure::OPERATIONAL)
		{
			idle(true);
		}

		if (idle() && mMine->active())
		{
			if (!mStoragePool.atCapacity())
				idle(false);
		}

		if(mMine->active())
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

protected:
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