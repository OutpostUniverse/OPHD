#include "Structure.h"

using namespace std;
#include "../../Constants.h"

/**
 * C'tor
 */
Structure::Structure(const string& name, const string& sprite_path, StructureClass _t):	Thing(name, sprite_path),
																						mId((long)(this)), // naive
																						mStructureClass(_t)
{
	mPopulationRequirements.fill(0);
}


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
	disabledStateSet();
}


/**
* Sets an Operational state for the Structure.
*/
void Structure::enable()
{
	if (forceIdle()) { idle(); return; }

	sprite().resume();
	sprite().color(255, 255, 255, 255);
	state(OPERATIONAL);
}


/**
* Sets idle state of the Structure.
*/
void Structure::idle()
{
	if (forceIdle()) { return; }

	sprite().pause();
	sprite().color(255, 255, 255, 185);
	state(IDLE);
}


/**
 * Forces an idle state. Used to prevent automatic enabling of the
 * Structure.
 */
void Structure::forceIdle(bool force)
{
	if (disabled() || destroyed()) { return; }

	// Note that the order in which the flag is set matters
	// in terms of the logic involved here.
	if (force)
	{
		idle();
		mForcedIdle = true;
	}
	else
	{
		mForcedIdle = false;
		enable();
	}
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
 * Provided for loading purposes.
 */
void Structure::forced_state_change(StructureState _s)
{
	defineResourceInput();
	defineResourceOutput();

	if (age() >= turnsToBuild())
	{
		sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
		//enable();
	}

	if (_s == OPERATIONAL)
		enable();
	if (_s == IDLE)
		idle();
	if (_s == DISABLED)
		disable();
	if (_s == DESTROYED)
		destroy();
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
 * 
 * \throws	Throws \c std::runtime_error
 */
void Structure::die()
{
	Thing::die();

	#if defined(_DEBUG)
	cout << "Holy shit, a Structure died!!!" << endl;
	throw std::runtime_error("Thing::die() was called on a Structure!");
	#endif
}
