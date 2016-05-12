#include "Structure.h"

using namespace std;
#include "../../Constants.h"

/**
 * C'tor
 */
Structure::Structure(const string& name, const string& sprite_path, StructureType _t):	Thing(name, sprite_path),
																						mId((int)this), // naive
																						mTurnsToBuild(0),
																						mAge(0),
																						mMaxAge(0),
																						mStructureState(UNDER_CONSTRUCTION),
																						mStructurePriority(PRIORITY_MEDIUM),
																						mStructureType(_t),
																						mConnectorDirection(CONNECTOR_INTERSECTION),
																						mRepairable(true),
																						mConnector(false),
																						mRequiresCHAP(true), // Most structures require a CHAP facility in order to operate.
																						mProvidesCHAP(false),
																						mSelfSustained(false),
																						mIsFactory(false)
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
void Structure::input(ResourcePool& _resourcePool)
{
	if(!enoughResourcesAvailable(_resourcePool))
	{
		enabled(false);
		return;
	}

	_resourcePool -= resourcesIn();
}


bool Structure::enoughResourcesAvailable(ResourcePool& r)
{
	return r >= mResourcesInput;
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


/**
 * Updates age of the structure and performs some basic age management logic.
 */
void Structure::incrementAge()
{
	mAge++;

	if (age() == turnsToBuild())
	{
		sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
		enabled(true);
		activate();
	}
	else if (age() == maxAge())
	{
		destroy();
	}
}


void Structure::update()
{
	incrementAge();
	think();
}

/**
* Sets a destroyed state.
*
* \note		This is permanent.
*/
void Structure::destroy()
{
	sprite().play(constants::STRUCTURE_STATE_DESTROYED);
	mStructureState = DESTROYED;

	// Destroyed buildings just need to be rebuilt right?
	repairable(false);
}
