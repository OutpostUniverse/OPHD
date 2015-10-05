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
		// FIXME:	Currently set to go to operational immediately.
		//			Should have several turns of construction before
		//			becoming operational.
		sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
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

		/** \todo	Currently mine production values are set based on their
		  *			yeild level. There should be some scalar effect based on
		  *			age of the mine such that over time the mine becomes less
		  *			and less productive instead of one day just not producing
		  *			anything anymore.
		  */
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