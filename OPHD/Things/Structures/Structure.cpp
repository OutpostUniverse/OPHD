// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Structure.h"

#include "../../Constants.h"


/**
 * Translation table for Structure States.
 */
std::map<Structure::StructureState, std::string> STRUCTURE_STATE_TRANSLATION =
{
	{ Structure::StructureState::UNDER_CONSTRUCTION, "Under Construction" },
	{ Structure::StructureState::OPERATIONAL, "Operational" },
	{ Structure::StructureState::IDLE, "Idle" },
	{ Structure::StructureState::DISABLED, "Disabled" },
	{ Structure::StructureState::DESTROYED, "Destroyed" },
};


/**
 * Translation table for Structure Classes.
 */
std::map<Structure::StructureClass, std::string> STRUCTURE_CLASS_TRANSLATION =
{
	{ Structure::StructureClass::CLASS_COMMAND, "Command" },
	{ Structure::StructureClass::CLASS_COMM, "Communication" },
	{ Structure::StructureClass::CLASS_COMMERCIAL, "Commercial" },
	{ Structure::StructureClass::CLASS_ENERGY_PRODUCTION, "Energy Production" },
	{ Structure::StructureClass::CLASS_FACTORY, "Factory" },
	{ Structure::StructureClass::CLASS_FOOD_PRODUCTION, "Food Production" },
	{ Structure::StructureClass::CLASS_LABORATORY, "Laboratory" },
	{ Structure::StructureClass::CLASS_LANDER, "Lander" },
	{ Structure::StructureClass::CLASS_LIFE_SUPPORT, "Life Support" },
	{ Structure::StructureClass::CLASS_MINE, "Mine Facility" },
	{ Structure::StructureClass::CLASS_PARK, "Park / Reservoir" },
	{ Structure::StructureClass::CLASS_SURFACE_POLICE, "Police" },
	{ Structure::StructureClass::CLASS_UNDERGROUND_POLICE, "Police" },
	{ Structure::StructureClass::CLASS_RECREATION_CENTER, "Recreation Center" },
	{ Structure::StructureClass::CLASS_RECYCLING, "Recycling" },
	{ Structure::StructureClass::CLASS_RESIDENCE, "Residential" },
	{ Structure::StructureClass::CLASS_SMELTER, "Raw Ore Processing" },
	{ Structure::StructureClass::CLASS_STORAGE, "Storage" },
	{ Structure::StructureClass::CLASS_TUBE, "Tube" },
	{ Structure::StructureClass::CLASS_UNDEFINED, "UNDEFINED" },
	{ Structure::StructureClass::CLASS_UNIVERSITY, "University" }
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
Structure::Structure(const std::string& name, const std::string& spritePath, StructureClass structureClass) :
	Thing(name, spritePath),
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
	state(StructureState::DISABLED);
	mDisabledReason = reason;
	mIdleReason = IdleReason::IDLE_NONE;
	disabledStateSet();
}


/**
* Sets an Operational state for the Structure.
*/
void Structure::enable()
{
	if (forceIdle())
	{
		idle(IdleReason::IDLE_PLAYER_SET);
		return;
	}

	sprite().resume();
	sprite().color(NAS2D::Color::White);
	state(StructureState::OPERATIONAL);
	mDisabledReason = DisabledReason::DISABLED_NONE;
	mIdleReason = IdleReason::IDLE_NONE;
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
	state(StructureState::IDLE);
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
		idle(IdleReason::IDLE_PLAYER_SET);
		mForcedIdle = true;
	}
	else
	{
		mForcedIdle = false;
		enable();
	}
}


bool Structure::enoughResourcesAvailable(ResourcePool& resourcePool)
{
	return resourcePool >= resourcesIn();
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
	state(StructureState::DESTROYED);

	// Destroyed buildings just need to be rebuilt right?
	repairable(false);
}


/**
 * Provided for loading purposes.
 */
void Structure::forced_state_change(StructureState structureState, DisabledReason disabledReason, IdleReason idleReason)
{
	defineResourceInput();
	defineResourceOutput();

	if (age() >= turnsToBuild())
	{
		sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
		//enable();
	}

	if (structureState == StructureState::OPERATIONAL) { enable(); }
	else if (structureState == StructureState::IDLE) { idle(idleReason); }
	else if (structureState == StructureState::DISABLED) { disable(disabledReason); }
	else if (structureState == StructureState::DESTROYED) { destroy(); }
	else if (structureState == StructureState::UNDER_CONSTRUCTION) { mStructureState = StructureState::UNDER_CONSTRUCTION; } // Kludge
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
