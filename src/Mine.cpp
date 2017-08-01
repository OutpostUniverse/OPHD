#include "Mine.h"

#include <iostream>

using namespace std;
using namespace NAS2D::Xml;


Mine::Mine(ProductionRate _rate):	mAge(0),
									mDepth(0),
									mCommonMetalYield(0),
									mRareMetalYield(0),
									mCommonMineralYield(0),
									mRareMineralYield(0),
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
			rareMetalsRate(7);
			commonMineralsRate(10);
			rareMineralsRate(7);
			break;

		case PRODUCTION_RATE_MEDIUM:
			commonMetalsRate(15);
			rareMetalsRate(10);
			commonMineralsRate(13);
			rareMineralsRate(10);
			break;

		case PRODUCTION_RATE_HIGH:
			commonMetalsRate(20);
			rareMetalsRate(16);
			commonMineralsRate(20);
			rareMineralsRate(16);
			break;
	}
}


void Mine::update()
{
	// FIXME: Make this gradually degrade the output of the Mine.
}


void Mine::serialize(XmlElement* _ti)
{
	_ti->attribute("age", age());
	_ti->attribute("depth", depth());
	_ti->attribute("active", active());
	_ti->attribute("exhausted", exhausted());
	_ti->attribute("yield", productionRate());
}
