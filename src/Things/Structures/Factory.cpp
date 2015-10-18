#include "Factory.h"


Factory::Factory(const string& name, const string& sprite_path):	Structure(name, sprite_path),
																	mProductionTime(0),
																	mProductionLength(0),
																	mResourcesPool(nullptr),
																	mRobotPool(nullptr)
{
	initFactory();
}


Factory::~Factory()
{}


void Factory::updateProduction()
{
	if (state() != StructureState::OPERATIONAL)
		return;

	++mProductionTime;

	if (mProductionTime >= mProductionLength)
	{
		resetProductionTime();
		productionComplete();
	}
}
