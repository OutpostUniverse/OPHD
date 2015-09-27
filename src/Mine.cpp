#include "Mine.h"

#include <iostream>

using namespace std;

Mine::Mine():	mAge(0),
				mDepth(0),
				mProductionRate(PRODUCTION_RATE_MEDIUM),
				mYieldsCommonMetals(false),
				mYieldsRareMetals(false),
				mYieldsCommonMinerals(false),
				mYieldsRareMinerals(false),
				mActive(false),
				mExhausted(false)

{}


Mine::Mine(ProductionRate _rate):	mAge(0),
									mDepth(0),
									mProductionRate(_rate),
									mYieldsCommonMetals(false),
									mYieldsRareMetals(false),
									mYieldsCommonMinerals(false),
									mYieldsRareMinerals(false),
									mActive(false),
									mExhausted(false)

{}


Mine::~Mine()
{}


void Mine::productionRate(ProductionRate _rate)
{
	mProductionRate = _rate;

	switch(mProductionRate)
	{
		case PRODUCTION_RATE_LOW:
			yieldsCommonMetals(true);
			yieldsRareMetals(false);
			yieldsCommonMinerals(true);
			yieldsRareMinerals(false);

			commonMetalsRate(5);
			rareMetalsRate(0);
			commonMineralsRate(2);
			rareMineralsRate(0);
			break;

		case PRODUCTION_RATE_MEDIUM:
			yieldsCommonMetals(true);
			yieldsRareMetals(true);
			yieldsCommonMinerals(true);
			yieldsRareMinerals(true);

			commonMetalsRate(10);
			rareMetalsRate(2);
			commonMineralsRate(5);
			rareMineralsRate(1);
			break;

		case PRODUCTION_RATE_HIGH:
			yieldsCommonMetals(true);
			yieldsRareMetals(true);
			yieldsCommonMinerals(true);
			yieldsRareMinerals(true);

			commonMetalsRate(15);
			rareMetalsRate(5);
			commonMineralsRate(8);
			rareMineralsRate(3);
			break;
	}
}


MineProduction Mine::update()
{
	MineProduction mp;

	mp.CommonMetalOre = commonMetalsRate();
	mp.RareMetalOre = rareMetalsRate();
	mp.CommonMineralOre = commonMineralsRate();
	mp.RareMineralOre = rareMineralsRate();

	return mp;
}