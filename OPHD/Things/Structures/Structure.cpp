// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Structure.h"

#include "../../Constants.h"


/**
 * Translation table for Structure States.
 */
std::map<StructureState, std::string> STRUCTURE_STATE_TRANSLATION =
{
	{ StructureState::UNDER_CONSTRUCTION, "Under Construction" },
	{ StructureState::OPERATIONAL, "Operational" },
	{ StructureState::IDLE, "Idle" },
	{ StructureState::DISABLED, "Disabled" },
	{ StructureState::DESTROYED, "Destroyed" },
};


/**
 * Translation table for Structure Classes.
 */
std::map<Structure::StructureClass, std::string> STRUCTURE_CLASS_TRANSLATION =
{
	{ Structure::StructureClass::Command, "Command" },
	{ Structure::StructureClass::Communication, "Communication" },
	{ Structure::StructureClass::Commercial, "Commercial" },
	{ Structure::StructureClass::EnergyProduction, "Energy Production" },
	{ Structure::StructureClass::Factory, "Factory" },
	{ Structure::StructureClass::FoodProduction, "Food Production" },
	{ Structure::StructureClass::Laboratory, "Laboratory" },
	{ Structure::StructureClass::Lander, "Lander" },
	{ Structure::StructureClass::LifeSupport, "Life Support" },
	{ Structure::StructureClass::Mine, "Mine Facility" },
	{ Structure::StructureClass::Park, "Park / Reservoir" },
	{ Structure::StructureClass::SurfacePolice, "Police" },
	{ Structure::StructureClass::UndergroundPolice, "Police" },
	{ Structure::StructureClass::RecreationCenter, "Recreation Center" },
	{ Structure::StructureClass::Recycling, "Recycling" },
	{ Structure::StructureClass::Residence, "Residential" },
	{ Structure::StructureClass::Smelter, "Raw Ore Processing" },
	{ Structure::StructureClass::Storage, "Storage" },
	{ Structure::StructureClass::Tube, "Tube" },
	{ Structure::StructureClass::Undefined, "UNDEFINED" },
	{ Structure::StructureClass::University, "University" }
};


const std::string& structureStateDescription(StructureState _state)
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
	Thing(name, spritePath, constants::STRUCTURE_STATE_CONSTRUCTION),
	mStructureClass(structureClass)
{
	mPopulationRequirements.fill(0);
	mPopulationAvailable.fill(0);
}


Structure::Structure(const std::string& name, const std::string& spritePath, const std::string& initialAction, StructureClass structureClass) :
	Thing(name, spritePath, initialAction),
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
	mIdleReason = IdleReason::None;
	disabledStateSet();
}


/**
* Sets an Operational state for the Structure.
*/
void Structure::enable()
{
	if (forceIdle())
	{
		idle(IdleReason::PlayerSet);
		return;
	}

	sprite().resume();
	sprite().color(NAS2D::Color::White);
	state(StructureState::OPERATIONAL);
	mDisabledReason = DisabledReason::None;
	mIdleReason = IdleReason::None;
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
	mDisabledReason = DisabledReason::None;
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
		idle(IdleReason::PlayerSet);
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
