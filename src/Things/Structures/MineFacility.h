#ifndef __MINE_FACILITY__
#define __MINE_FACILITY__

#include "Structure.h"

#include "../../Mine.h"

class MineFacility: public Structure
{
public:
	MineFacility(Mine* mine):	Structure("Mine Facility", "structures/mine_facility.sprite"),
								mMine(mine)
	{
		sprite().play("operational");
		maxAge(1200);
		mMine->active(true);
	}


	~MineFacility()
	{}

	void update()
	{
		if(!mMine->active() && state() == Structure::OPERATIONAL)
		{
			idle(true);
		}

		if(mMine->active())
		{
			MineProduction mp = mMine->update();

			mResourcesOutput.commonMetalsOre = mp.CommonMetalOre;
			mResourcesOutput.rareMetalsOre = mp.RareMetalOre;
			mResourcesOutput.commonMineralsOre = mp.CommonMineralOre;
			mResourcesOutput.rareMineralsOre = mp.RareMineralOre;
		}
	}

protected:
private:

	virtual void defineResourceOutput()
	{
	}

	virtual void defineResourceValue()
	{
		// Resource value if demolished.
		mResourceValue.commonMetals = 20;
		mResourceValue.rareMetals = 5;
	}


	Mine*		mMine;

};

#endif