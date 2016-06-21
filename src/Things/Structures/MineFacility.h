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
		storage().capacity(500);
		defineResourceCostToBuild();
	}


	~MineFacility()
	{}
	

protected:

	virtual void think()
	{

		if (isIdle() && mMine->active())
		{
			if (!storage().atCapacity())
				enable();
		}

		if (mMine->active())
		{
			if (storage().atCapacity())
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

	virtual void defineResourceInput()
	{
		mMine->active(true);
	}

	virtual void defineResourceOutput()
	{}

	// Counterintuitive because these structures can't be build directly but
	// basically this is defined so we can get a recycling value.
	virtual void defineResourceCostToBuild()
	{
		resourcesCostToBuild().commonMetals(20);
		resourcesCostToBuild().rareMetals(5);
		resourcesCostToBuild().commonMinerals(10);
		resourcesCostToBuild().rareMinerals(0);
	}


	Mine*			mMine;

	ResourcePool	mProductionPool;
};

#endif