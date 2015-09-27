#include "Structure.h"

Structure::Structure(const std::string& name, const std::string& sprite_path):	Thing(name, sprite_path),
																				mTurnsToBuild(0),
																				mAge(0),
																				mMaxAge(0),
																				mStructureState(UNDER_CONSTRUCTION),
																				mConnectorDirection(CONNECTOR_INTERSECTION),
																				mRepairable(true),
																				mConnector(false)
{}


Structure::~Structure()
{}


void Structure::enabled(bool _b)
{
	if(_b)
	{
		sprite().resume();
		mStructureState = OPERATIONAL;
	}
	else
	{
		mStructureState = DISABLED;
		sprite().pause();
	}
}


void Structure::idle(bool _b)
{
	if(_b)
	{
		mStructureState = IDLE;
		sprite().pause();
	}
	else
	{
		sprite().resume();
		mStructureState = OPERATIONAL;
	}
}


void Structure::input(Resources& _resourcePool)
{
	if(!enoughResourcesAvailable(_resourcePool))
	{
		enabled(false);
		return;
	}

	_resourcePool -= resourcesIn();
}


bool Structure::enoughResourcesAvailable(const Resources& r)
{
	if(	r.commonMetals >= mResourcesInput.commonMetals &&
		r.rareMetals >= mResourcesInput.rareMetals &&
		r.commonMinerals >= mResourcesInput.commonMinerals &&
		r.rareMinerals >= mResourcesInput.rareMinerals &&
		r.energy >= mResourcesInput.energy)
		return true;

	return false;
}


/**
 * Called when a building is finished being built and its
 * resource requirements are defined.
 */
void Structure::activate()
{
	defineResourceInput();
	defineResourceOutput();
	defineResourceValue();
}