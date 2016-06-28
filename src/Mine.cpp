#include "Mine.h"

#include <iostream>

using namespace std;

Mine::Mine(ProductionRate _rate):	mAge(0),
									mDepth(0),
									mCommonMetalYieldRate(0),
									mRareMetalYieldRate(0),
									mCommonMineralYieldRate(0),
									mRareMineralYieldRate(0),
									mProductionRate(_rate),
									mActive(false),
									mExhausted(false)

{
	productionRate(_rate);
}


Mine::~Mine()
{}


void Mine::productionRate(ProductionRate _rate)
{
	mProductionRate = _rate;

	switch(mProductionRate)
	{
		case PRODUCTION_RATE_LOW:
			commonMetalsRate(10);
			rareMetalsRate(2);
			commonMineralsRate(8);
			rareMineralsRate(2);
			break;

		case PRODUCTION_RATE_MEDIUM:
			commonMetalsRate(15);
			rareMetalsRate(8);
			commonMineralsRate(12);
			rareMineralsRate(8);
			break;

		case PRODUCTION_RATE_HIGH:
			commonMetalsRate(20);
			rareMetalsRate(14);
			commonMineralsRate(18);
			rareMineralsRate(14);
			break;
	}
}


void Mine::update()
{
	// FIXME: Make this gradually degrade the output of the Mine.
}