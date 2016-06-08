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
 * Sets a Disabled state for the Structure.
 */
void Structure::disable()
{
	sprite().pause();
	sprite().color(255, 0, 0, 185);
	state(DISABLED);
}


/**
* Sets an Operational state for the Structure.
*/
void Structure::enable()
{
	sprite().resume();
	sprite().color(255, 255, 255, 255);
	state(OPERATIONAL);
}


/**
* Sets idle state of the Structure.
*/
void Structure::idle()
{
	sprite().pause();
	sprite().color(255, 255, 255, 185);
	state(IDLE);
}


/**
 * 
 */
void Structure::input(ResourcePool& _resourcePool)
{
	if(!enoughResourcesAvailable(_resourcePool))
	{
		disable();
		return;
	}

	_resourcePool -= resourcesIn();
}


bool Structure::enoughResourcesAvailable(ResourcePool& r)
{
	return r >= mResourcesInput;
}


/**
 * Called when a building is finished being built.
 * 
 * Sets the animation state of the Structure to Operational,
 * sets the building state to Opeational and sets resource
 * requirements.
 */
void Structure::activate()
{
	sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
	enable();

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
		activate();
	}
	else if (age() == maxAge())
	{
		destroy();
	}
}


void Structure::update()
{
	if (destroyed())
		return;

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
	state(DESTROYED);

	// Destroyed buildings just need to be rebuilt right?
	repairable(false);
}
