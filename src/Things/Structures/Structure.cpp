#include "Structure.h"


/**
 * C'tor
 */
Structure::Structure(const std::string& name, const std::string& sprite_path):	Thing(name, sprite_path),
																				mTurnsToBuild(0),
																				mAge(0),
																				mMaxAge(0),
																				mStructureState(UNDER_CONSTRUCTION),
																				mConnectorDirection(CONNECTOR_INTERSECTION),
																				mRepairable(true),
																				mConnector(false),
																				mRequiresCHAP(true) // Most structures require a CHAP facility in order to operate.
{}


/**
 * D'tor
 */
Structure::~Structure()
{}


/**
 * Sets enabled state of the Structure.
 * 
 * Enabled is distinctly different than Idle even though the Structure ultimately
 * isn't producing. Disabled structures are disabled either due to damage or not
 * enough resources. In either case, this will have a negative effect on morale.
 * 
 * \param	_b	True starts animation playback and sets operational state. Otherwise stops animation and sets disabled state.
 */
void Structure::enabled(bool _b)
{
	if(_b)
	{
		sprite().resume();
		mStructureState = OPERATIONAL;
	}
	else
	{
		sprite().pause();
		mStructureState = DISABLED;
	}
}


/**
* Sets idle state of the Structure.
* 
* Idle is distinctly different than disabled even though the Structure ultimately
* isn't producing. Idle Structures have been put into a state of low energy consumption
* by the Player and not because they have been damaged or lack resources. Structures in
* this state have no effect on Morale.
* 
* \param	_b	True starts animation playback and sets operational state. Otherwise stops animation and sets idle state.
*/
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


/**
 * 
 */
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