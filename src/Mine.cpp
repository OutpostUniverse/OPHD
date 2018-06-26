// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Mine.h"

#include <iostream>

using namespace std;
using namespace NAS2D::Xml;


Mine::Mine(MineProductionRate _rate) : mProductionRate(_rate)
{
	productionRate(_rate);
}


Mine::~Mine()
{}


void Mine::productionRate(MineProductionRate _rate)
{
	mProductionRate = _rate;

	switch(mProductionRate)
	{
		case PRODUCTION_RATE_LOW:
			commonMetalsRate(150);
			rareMetalsRate(125);
			commonMineralsRate(150);
			rareMineralsRate(125);
			break;

		case PRODUCTION_RATE_MEDIUM:
			commonMetalsRate(200);
			rareMetalsRate(175);
			commonMineralsRate(200);
			rareMineralsRate(175);
			break;

		case PRODUCTION_RATE_HIGH:
			commonMetalsRate(250);
			rareMetalsRate(200);
			commonMineralsRate(250);
			rareMineralsRate(200);
			break;
	}
}


bool Mine::exhausted() const
{
	return mCommonMetalYield == 0 && mRareMetalYield == 0 && mCommonMineralYield == 0 && mRareMineralYield == 0;
}


void Mine::update()
{
	// FIXME: Make this gradually degrade the output of the Mine.
}


void Mine::serialize(XmlElement* _ti)
{
	_ti->attribute("depth", depth());
	_ti->attribute("active", active());
	_ti->attribute("yield", productionRate());
}
