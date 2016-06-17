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
	}


	~MineFacility()
	{}
	

protected:

	virtual void think()
	{

		if (isIdle() && mMine->active())
		{
			if (!mStoragePool.atCapacity())
				enable();
		}

		if (mMine->active())
		{
			if (mStoragePool.atCapacity())
			{
				idle();
				return;
			}

			mMine->update();

			mProductionPool.pushResource(ResourcePool::RESOURCE_COMMON_METALS_ORE, mMine->commonMetalsRate());
			mProductionPool.pushResource(ResourcePool::RESOURCE_COMMON_MINERALS_ORE, mMine->commonMineralsRate());
			mProductionPool.pushResource(ResourcePool::RESOURCE_RARE_METALS_ORE, mMine->rareMetalsRate());
			mProductionPool.pushResource(ResourcePool::RESOURCE_RARE_MINERALS_ORE, mMine->rareMineralsRate());

			storage().pushResources(mProductionPool);
		}
		else if(!isIdle())
		{
			idle();
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

		// This function is called when the Mine Facility activates so
		// I'm activating the mine here.
		mMine->active(true);
	}


	Mine*			mMine;

	ResourcePool	mProductionPool;
};

#endif