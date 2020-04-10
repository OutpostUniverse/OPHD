// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Structure.h"

#include "../../Constants.h"


/**
 * Translation table for Structure States.
 */
std::map<Structure::StructureState, std::string> STRUCTURE_STATE_TRANSLATION =
{
	{ Structure::UNDER_CONSTRUCTION,	"Under Construction" },
	{ Structure::OPERATIONAL,			"Operational" },
	{ Structure::IDLE,					"Idle" },
	{ Structure::DISABLED,				"Disabled" },
	{ Structure::DESTROYED,				"Destroyed" },
};


/**
 * Translation table for Structure Classes.
 */
std::map<Structure::StructureClass, std::string> STRUCTURE_CLASS_TRANSLATION =
{
	{ Structure::CLASS_COMMAND,				"Command" },
	{ Structure::CLASS_COMM,				"Communication" },
	{ Structure::CLASS_COMMERCIAL,			"Commercial" },
	{ Structure::CLASS_ENERGY_PRODUCTION,	"Energy Production" },
	{ Structure::CLASS_FACTORY,				"Factory" },
	{ Structure::CLASS_FOOD_PRODUCTION,		"Food Production" },
	{ Structure::CLASS_LABORATORY,			"Laboratory" },
	{ Structure::CLASS_LANDER,				"Lander" },
	{ Structure::CLASS_LIFE_SUPPORT,		"Life Support" },
	{ Structure::CLASS_MINE,				"Mine Facility" },
	{ Structure::CLASS_PARK,				"Park / Reservoir" },
	{ Structure::CLASS_SURFACE_POLICE,		"Police" },
	{ Structure::CLASS_UNDERGROUND_POLICE,	"Police" },
	{ Structure::CLASS_RECREATION_CENTER,	"Recreation Center" },
	{ Structure::CLASS_RECYCLING,			"Recycling" },
	{ Structure::CLASS_RESIDENCE,			"Residential" },
	{ Structure::CLASS_SMELTER,				"Raw Ore Processing" },
	{ Structure::CLASS_STORAGE,				"Storage" },
	{ Structure::CLASS_TUBE,				"Tube" },
	{ Structure::CLASS_UNDEFINED,			"UNDEFINED" },
	{ Structure::CLASS_UNIVERSITY,			"University" }
};


const std::string& structureStateDescription(Structure::StructureState _state)
{
	return STRUCTURE_STATE_TRANSLATION[_state];
}


const std::string& structureClassDescription(Structure::StructureClass _class)
{
	return STRUCTURE_CLASS_TRANSLATION[_class];
}


/**
 * C'tor
 */
Structure::Structure(const std::string& name, const std::string& spritePath, StructureClass structureClass):	Thing(name, spritePath),
	mStructureClass(structureClass)

{
	mPopulationRequirements.fill(0);
	mPopulationAvailable.fill(0);
}


/**
 * Sets a Disabled state for the Structure.
 */
void Structure::disable(DisabledReason reason)
{
	sprite().pause();
	sprite().color(NAS2D::Color{255, 0, 0, 185});
	state(DISABLED);
	mDisabledReason = reason;
	mIdleReason = IDLE_NONE;
	disabledStateSet();
}


/**
* Sets an Operational state for the Structure.
*/
void Structure::enable()
{
	if (forceIdle())
	{
		idle(IDLE_PLAYER_SET);
		return;
	}

	sprite().resume();
	sprite().color(NAS2D::Color::White);
	state(OPERATIONAL);
	mDisabledReason = DisabledReason::DISABLED_NONE;
	mIdleReason = IDLE_NONE;
}


/**
* Sets idle state of the Structure.
*/
void Structure::idle(IdleReason reason)
{
	if (forceIdle() && !disabled())
	{
		return;
	}

	sprite().pause();
	sprite().color(NAS2D::Color{255, 255, 255, 185});
	mDisabledReason = DisabledReason::DISABLED_NONE;
	mIdleReason = reason;
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
		idle(IDLE_PLAYER_SET);
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

	activated();
}


void Structure::update()
{
	if (disabled() || destroyed()) { return; }
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
void Structure::forced_state_change(StructureState _s, DisabledReason _dr, IdleReason _ir)
{
	defineResourceInput();
	defineResourceOutput();

	if (age() >= turnsToBuild())
	{
		sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
		//enable();
	}

	if (_s == OPERATIONAL)				{ enable(); }
	else if (_s == IDLE)				{ idle(_ir); }
	else if (_s == DISABLED)			{ disable(_dr); }
	else if (_s == DESTROYED)			{ destroy(); }
	else if (_s == UNDER_CONSTRUCTION)	{ mStructureState = UNDER_CONSTRUCTION; } // Kludge
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
	std::cout << "Holy shit, a Structure died!!!" << std::endl;
	throw std::runtime_error("Thing::die() was called on a Structure!");
	#endif
}
