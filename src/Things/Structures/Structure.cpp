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
																						mStructureType(_t),
																						mConnectorDirection(CONNECTOR_INTERSECTION),
																						mRepairable(true),
																						mRequiresCHAP(true), // Most structures require a CHAP facility in order to operate.
																						mSelfSustained(false)
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


bool Structure::enoughResourcesAvailable(ResourcePool& r)
{
	return r >= resourcesIn();
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
}


void Structure::update()
{
	if (disabled() || destroyed())
		return;
	
	incrementAge();
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


/**
 * Returns a ResourcePool containing recycling values for the Structure.
 * 
 * \note	Not very efficient but it's not called often so whatever.
 */
ResourcePool Structure::resourcesRecyclingValue()
{
	ResourcePool rp;
	rp.commonMetals(resourcesCostToBuild().commonMetals() * 0.9f);
	rp.rareMetals(resourcesCostToBuild().rareMetals() * 0.9f);
	rp.commonMinerals(resourcesCostToBuild().commonMinerals() * 0.9f);
	rp.rareMinerals(resourcesCostToBuild().rareMinerals() * 0.9f);

	return rp;
}

/**
 * Special overidding of Thing::die for Structure.
 * 
 * There is no conceivable situation in which a Structure should be marked as 'dead' or have its
 * 'die' function be called. Such cases should be treated as bad logic and immediately and very
 * loudly fail.
 * 
 * This function exists purely for debug purposes as it was noticed in StructureManager testing
 * for this flag when there should never be a need to do so.
 * 
 * \note	This is for debug purposes only. Release modes will silently ignore this condition
 *			and simply act as a passthrough.
 */
void Structure::die()
{
	Thing::die();

	#ifdef _DEBUG
	cout << "Holy shit, a Structure died!!!" << endl;
	throw Exception(666, "Dead Structure", "Thing::die() was called on a Structure!");
	#endif
}